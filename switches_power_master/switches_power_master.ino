#include <Wire.h>
#include <VL53L0X.h>
#include <Servo.h>

//  SENSORS
VL53L0X outerSensor;
VL53L0X innerSensor;

#define XSHUT_PIN_1 3 //inner is digital pin 2
#define XSHUT_PIN_2 2 //outer is digital pin 3

//  SERVOS
Servo innerServo;
Servo outerServo;

//  STATE
int innerState = 1;
int outerState = 1;

unsigned long innerTimer = 0;
unsigned long outerTimer = 0;

unsigned long innerInterval = 5000;
unsigned long outerInterval = 5000;

bool started = false;
unsigned long startTime = 0;

//  SEMANTIC FLAGS
bool inner_tree_lowered = true;
bool outer_tree_lowered = true;

// Obstacle states — updated by runActuation(), read by loop() for switch rerouting.
bool outer_tree_fallen = false;
bool inner_tree_fallen = false;

//  OBSTACLE / TRACK POWER
// Pins 4 and 5 used (pin 2 is taken by XSHUT_PIN_2, pin 3 by XSHUT_PIN_1).
// INPUT_PULLUP: pin reads HIGH normally, LOW when obstacle detected.
#define INNER_OBSTACLE_SENSOR_PIN 4  // track 4 — inner obstacle
#define OUTER_OBSTACLE_SENSOR_PIN 5  // track 5 — outer obstacle

// Relays are active LOW: LOW = relay energized = track power CUT, HIGH = relay off = track power ON.
// A1 = ATmega physical pin 24 = Relay 1 -> Track 4 (inner)
// A2 = ATmega physical pin 25 = Relay 2 -> Track 5 (outer)
#define TRACK_POWER_RELAY_1 A1  // track 4, inner obstacle
#define TRACK_POWER_RELAY_2 A2  // track 5, outer obstacle

//  TRACK SWITCHES
// Switch 1: DIR=false -> default straight, DIR=true -> exit loop to default straight
// Switch 2: DIR=false -> default,           DIR=true -> route to inner loop (track 4)
// Switch 3: DIR=false -> outer -> default,   DIR=true -> inner -> default
// Switch 4: DIR=false -> default exit board, DIR=true -> route to outer loop (track 5)
const int DIR1  = 4;   // physical pin 6
const int TRIG1 = 6;   // physical pin 12
const int DIR2  = 20;  // physical pin 9
const int TRIG2 = 7;   // physical pin 13
const int DIR3  = 21;  // physical pin 10
const int TRIG3 = 12;  // physical pin 18
const int DIR4  = 5;   // physical pin 11
const int TRIG4 = A0;  // physical pin 23

// ConOps phase durations
#define OUTER_DURATION_MS 60000UL
#define INNER_DURATION_MS 60000UL

//  SETUP
void setup() {
  Serial.begin(9600);
  delay(3000);

  Serial.println("A: START");

  // servos
  innerServo.attach(11);
  outerServo.attach(10);

  innerServo.write(90); // lowered (inner)
  outerServo.write(0);  // lowered (outer)

  // sensors
  pinMode(XSHUT_PIN_1, OUTPUT);
  pinMode(XSHUT_PIN_2, OUTPUT);
  digitalWrite(XSHUT_PIN_1, LOW);
  digitalWrite(XSHUT_PIN_2, LOW);
  delay(500);

  Wire.begin();
  Wire.setClock(400000);

  digitalWrite(XSHUT_PIN_1, HIGH);
  delay(500);
  outerSensor.init();
  outerSensor.setAddress(0x30);
  outerSensor.startContinuous();

  digitalWrite(XSHUT_PIN_2, HIGH);
  delay(500);
  innerSensor.init();
  innerSensor.setAddress(0x31);
  innerSensor.startContinuous();

  Serial.println("F: BOTH SYSTEMS RUNNING");

  // obstacle sensors: internal pull-up so idle state reads HIGH (no obstacle)
  pinMode(INNER_OBSTACLE_SENSOR_PIN, INPUT_PULLUP);
  pinMode(OUTER_OBSTACLE_SENSOR_PIN, INPUT_PULLUP);

  // relays: initialize LOW so both tracks have power ON at startup (relay off = power flows)
  pinMode(TRACK_POWER_RELAY_1, OUTPUT);
  digitalWrite(TRACK_POWER_RELAY_1, LOW);
  pinMode(TRACK_POWER_RELAY_2, OUTPUT);
  digitalWrite(TRACK_POWER_RELAY_2, LOW);

  // track switches: all OUTPUT, default position
  pinMode(DIR1,  OUTPUT); digitalWrite(DIR1,  LOW);
  pinMode(TRIG1, OUTPUT); digitalWrite(TRIG1, LOW);
  pinMode(DIR2,  OUTPUT); digitalWrite(DIR2,  LOW);
  pinMode(TRIG2, OUTPUT); digitalWrite(TRIG2, LOW);
  pinMode(DIR3,  OUTPUT); digitalWrite(DIR3,  LOW);
  pinMode(TRIG3, OUTPUT); digitalWrite(TRIG3, LOW);
  pinMode(DIR4,  OUTPUT); digitalWrite(DIR4,  LOW);
  pinMode(TRIG4, OUTPUT); digitalWrite(TRIG4, LOW);

  // throw switches to outer loop position sequentially — 2s apart to avoid current surge
  moveSwitch(DIR1, TRIG1, false); delay(2000);
  moveSwitch(DIR2, TRIG2, false); delay(2000);
  moveSwitch(DIR4, TRIG4, true);  delay(2000);
  moveSwitch(DIR3, TRIG3, false);
  Serial.println("SWITCHES: initialized to outer loop");

  startTime = millis();
}

// Fires a 120ms pulse on trigPin to actuate the solenoid switch.
void pulse(int trigPin) {
  digitalWrite(trigPin, HIGH);
  delay(120);
  digitalWrite(trigPin, LOW);
}

// Sets DIR pin then fires TRIG pulse to throw a switch in the given direction.
void moveSwitch(int dirPin, int trigPin, bool dir) {
  digitalWrite(dirPin, dir);
  delay(2);
  pulse(trigPin);
}

// SW1=LOW, SW2=LOW, SW4=HIGH, SW3=LOW -> outer loop (trk5)
void setOuterSwitches() {
  moveSwitch(DIR1, TRIG1, false);
  moveSwitch(DIR2, TRIG2, false);
  moveSwitch(DIR4, TRIG4, true);
  moveSwitch(DIR3, TRIG3, false);
  Serial.println("SWITCHES: outer loop");
}

// SW1=HIGH, SW2=LOW, SW4=LOW, SW3=HIGH -> inner loop (trk4)
void setInnerSwitches() {
  moveSwitch(DIR1, TRIG1, true);
  moveSwitch(DIR2, TRIG2, false);
  moveSwitch(DIR4, TRIG4, false);
  moveSwitch(DIR3, TRIG3, true);
  Serial.println("SWITCHES: inner loop");
}

// SW1=HIGH, SW2=LOW, SW4=LOW, SW3=LOW -> exit board
void setExitSwitches() {
  moveSwitch(DIR1, TRIG1, true);
  moveSwitch(DIR2, TRIG2, false);
  moveSwitch(DIR4, TRIG4, false);
  moveSwitch(DIR3, TRIG3, false);
  Serial.println("SWITCHES: exit");
}

// Runs one cycle of sensor read, relay control, servo actuation, and telemetry.
// Updates global outer_tree_fallen / inner_tree_fallen for loop() to read.
void runActuation() {
  int outerDist = outerSensor.readRangeContinuousMillimeters();
  int innerDist = innerSensor.readRangeContinuousMillimeters();

  outer_tree_fallen = (outerDist <= 170);
  inner_tree_fallen = (innerDist <= 120);
  bool both_tree_fallen = outer_tree_fallen && inner_tree_fallen;

  // Track 4 — inner obstacle -> Relay 1 (A1)
  if (inner_tree_fallen == 1) {
    digitalWrite(TRACK_POWER_RELAY_1, HIGH);
    Serial.println("INNER OBSTACLE DETECTED — track 4 power CUT");
  } else {
    digitalWrite(TRACK_POWER_RELAY_1, LOW);
    Serial.println("Track 4 clear — power ON");
  }

  // Track 5 — outer obstacle -> Relay 2 (A2)
  if (outer_tree_fallen == 1) {
    digitalWrite(TRACK_POWER_RELAY_2, HIGH);
    Serial.println("OUTER OBSTACLE DETECTED — track 5 power CUT");
  } else {
    digitalWrite(TRACK_POWER_RELAY_2, LOW);
    Serial.println("Track 5 clear — power ON");
  }

  // inner servo
  if (millis() - innerTimer >= innerInterval) {
    innerTimer = millis();
    innerState = !innerState;
    innerServo.write(innerState ? 90 : 0);
    inner_tree_lowered = innerState;
    innerInterval = random(5000, 10000);
  }

  // outer servo
  if (millis() - outerTimer >= outerInterval) {
    outerTimer = millis();
    outerState = !outerState;
    outerServo.write(outerState ? 0 : 90); // IMPORTANT: inverted mapping
    outer_tree_lowered = outerState;
    outerInterval = random(5000, 10000);
  }

  Serial.print("inner_dist=");   Serial.print(innerDist);
  Serial.print(" outer_dist=");  Serial.print(outerDist);
  Serial.print(" | INNER_FALLEN="); Serial.print(inner_tree_fallen);
  Serial.print(" OUTER_FALLEN=");   Serial.print(outer_tree_fallen);
  Serial.print(" | INNER_ACTUATED="); Serial.print(inner_tree_lowered);
  Serial.print(" OUTER_ACTUATED=");   Serial.print(outer_tree_lowered);
  Serial.print(" BOTH FALLEN ="); Serial.println(both_tree_fallen);

  delay(500);
}

//  LOOP
void loop() {

  // ================= HOLD PHASE =================
  if (!started && (millis() - startTime < 5000)) {
    Serial.println("hold_phase=1");
    return;
  }

  if (!started) {
    innerTimer    = millis();
    outerTimer    = millis();
    innerInterval = random(1000, 5000);
    outerInterval = random(5000, 10000);
    started = true;
  }

  // ================= OUTER LOOP — 60 seconds =================
  // If outer track blocked, reroute to inner switches; restore when clear.
  setOuterSwitches();
  bool outerRerouted = false;
  unsigned long t = millis();
  while (millis() - t < OUTER_DURATION_MS) {
    runActuation();
    if (outer_tree_fallen && !outerRerouted) {
      setInnerSwitches();
      outerRerouted = true;
    } else if (!outer_tree_fallen && outerRerouted) {
      setOuterSwitches();
      outerRerouted = false;
    }
  }
  if (outerRerouted) setOuterSwitches(); // restore before transitioning

  // ================= INNER LOOP — 60 seconds =================
  // If inner track blocked, reroute to outer switches; restore when clear.
  setInnerSwitches();
  bool innerRerouted = false;
  t = millis();
  while (millis() - t < INNER_DURATION_MS) {
    runActuation();
    if (inner_tree_fallen && !innerRerouted) {
      setOuterSwitches();
      innerRerouted = true;
    } else if (!inner_tree_fallen && innerRerouted) {
      setInnerSwitches();
      innerRerouted = false;
    }
  }
  if (innerRerouted) setInnerSwitches(); // restore before transitioning

  // ================= EXIT =================
  setExitSwitches();
  while (true) {
    runActuation();
  }
}
