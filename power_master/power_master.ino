#include <Wire.h>
#include <VL53L0X.h>
#include <Servo.h>

// ---- SENSORS ----
VL53L0X outerSensor;
VL53L0X innerSensor;

// XSHUT_OUTER (pin 3) brings up outer sensor first -> assigned 0x30
// XSHUT_INNER (pin 2) brings up inner sensor second -> assigned 0x31
#define XSHUT_OUTER 3
#define XSHUT_INNER 2

// ---- SERVOS ----
Servo innerServo;
Servo outerServo;

int innerState = 1;
int outerState = 1;

unsigned long innerTimer    = 0;
unsigned long outerTimer    = 0;
unsigned long innerInterval = 5000;
unsigned long outerInterval = 5000;

bool started   = false;
unsigned long startTime = 0;

bool inner_tree_lowered = true;
bool outer_tree_lowered = true;

// ---- TRACK POWER RELAYS ----
// LOW = relay off = power ON | HIGH = relay on = power CUT
// A1 -> inner track (track 4)
// A2 -> outer track (track 5)
#define TRACK_POWER_RELAY_1 A1
#define TRACK_POWER_RELAY_2 A2

// ---- SETUP ----
void setup() {
  Serial.begin(9600);
  delay(3000);
  Serial.println(F("A: START"));

  // Servos
  innerServo.attach(11);
  outerServo.attach(10);
  innerServo.write(90);   // lowered (inner)
  outerServo.write(0);    // lowered (outer)

  // Sensors — bring up one at a time to assign I2C addresses
  pinMode(XSHUT_OUTER, OUTPUT);
  pinMode(XSHUT_INNER, OUTPUT);
  digitalWrite(XSHUT_OUTER, LOW);
  digitalWrite(XSHUT_INNER, LOW);
  delay(500);

  Wire.begin();
  Wire.setClock(400000);

  digitalWrite(XSHUT_OUTER, HIGH); delay(500);
  outerSensor.init();
  outerSensor.setAddress(0x30);
  outerSensor.startContinuous();

  digitalWrite(XSHUT_INNER, HIGH); delay(500);
  innerSensor.init();
  innerSensor.setAddress(0x31);
  innerSensor.startContinuous();

  Serial.println(F("F: BOTH SYSTEMS RUNNING"));

  // Relays — both tracks powered ON at startup
  pinMode(TRACK_POWER_RELAY_1, OUTPUT); digitalWrite(TRACK_POWER_RELAY_1, LOW);
  pinMode(TRACK_POWER_RELAY_2, OUTPUT); digitalWrite(TRACK_POWER_RELAY_2, LOW);

  startTime = millis();
}

// ---- LOOP ----
void loop() {

  // Hold phase
  if (!started && (millis() - startTime < 5000)) {
    Serial.println(F("hold_phase=1"));
    return;
  }

  if (!started) {
    innerTimer    = millis();
    outerTimer    = millis();
    innerInterval = random(1000, 5000);
    outerInterval = random(5000, 10000);
    started = true;
  }

  // Sensor read
  int outerDist = outerSensor.readRangeContinuousMillimeters();
  int innerDist = innerSensor.readRangeContinuousMillimeters();

  bool outer_tree_fallen = (outerDist <= 170);
  bool inner_tree_fallen = (innerDist <= 120);
  bool both_tree_fallen  = outer_tree_fallen && inner_tree_fallen;

  // Track 4 — inner relay
  if (inner_tree_fallen) {
    digitalWrite(TRACK_POWER_RELAY_1, HIGH);
    Serial.println(F("INNER OBSTACLE DETECTED - track 4 power CUT"));
  } else {
    digitalWrite(TRACK_POWER_RELAY_1, LOW);
    Serial.println(F("Track 4 clear - power ON"));
  }

  // Track 5 — outer relay
  if (outer_tree_fallen) {
    digitalWrite(TRACK_POWER_RELAY_2, HIGH);
    Serial.println(F("OUTER OBSTACLE DETECTED - track 5 power CUT"));
  } else {
    digitalWrite(TRACK_POWER_RELAY_2, LOW);
    Serial.println(F("Track 5 clear - power ON"));
  }

  // Inner servo tick
  if (millis() - innerTimer >= innerInterval) {
    innerTimer    = millis();
    innerState    = !innerState;
    innerServo.write(innerState ? 90 : 0);
    inner_tree_lowered = innerState;
    innerInterval = random(5000, 10000);
  }

  // Outer servo tick
  if (millis() - outerTimer >= outerInterval) {
    outerTimer    = millis();
    outerState    = !outerState;
    outerServo.write(outerState ? 0 : 90);   // inverted mapping
    outer_tree_lowered = outerState;
    outerInterval = random(5000, 10000);
  }

  // Telemetry
  Serial.print(F("inner_dist="));    Serial.print(innerDist);
  Serial.print(F(" outer_dist="));   Serial.print(outerDist);
  Serial.print(F(" | INNER_FALLEN=")); Serial.print(inner_tree_fallen);
  Serial.print(F(" OUTER_FALLEN="));  Serial.print(outer_tree_fallen);
  Serial.print(F(" | INNER_ACT="));   Serial.print(inner_tree_lowered);
  Serial.print(F(" OUTER_ACT="));     Serial.print(outer_tree_lowered);
  Serial.print(F(" BOTH_FALLEN="));   Serial.println(both_tree_fallen);

  delay(500);
}
