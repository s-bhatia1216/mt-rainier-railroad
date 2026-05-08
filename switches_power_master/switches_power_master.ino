
// Mt. Rainier Railroad - Integrated Master Controller
//
// Subsystems:
//   1. VL53L0X distance sensors  - detect tree obstacles on each track
//   2. Servo-driven trees        - randomly lower/raise obstacles
//   3. Track power relays (A1/A2)- cut track power when tree has fallen
//   4. Solenoid track switches   - staggered active-LOW firing (QUAD_CONOPS)
//   5. ConOps                    - outer loop -> inner loop -> exit board


#include <Wire.h>
#include <VL53L0X.h>
#include <Servo.h>


// SECTION 1: SENSORS (VL53L0X)


VL53L0X outerSensor;
VL53L0X innerSensor;

// XSHUT_OUTER on pin 3 brings up the outer sensor first (assigned 0x30).
// XSHUT_INNER on pin 2 brings up the inner sensor second (assigned 0x31).
#define XSHUT_OUTER 3
#define XSHUT_INNER 2

#define OUTER_FALL_THRESHOLD_MM 170
#define INNER_FALL_THRESHOLD_MM 120

// Updated by runActuation() each cycle; read by loop() for switch rerouting.
bool outer_tree_fallen = false;
bool inner_tree_fallen = false;


// SECTION 2: SERVOS (tree actuation)


Servo innerServo;
Servo outerServo;

int innerState = 1;
int outerState = 1;

bool inner_tree_lowered = true;
bool outer_tree_lowered = true;

unsigned long innerTimer    = 0;
unsigned long outerTimer    = 0;
unsigned long innerInterval = 5000;
unsigned long outerInterval = 5000;


// SECTION 3: TRACK POWER RELAYS
//
// LOW  = relay off  = track power ON  (default / safe state)
// HIGH = relay on   = track power CUT (activated when tree fallen)
//
// A1 -> inner track (track 4)
// A2 -> outer track (track 5)


#define TRACK_POWER_RELAY_INNER A1
#define TRACK_POWER_RELAY_OUTER A2


// SECTION 4: TRACK SWITCHES (solenoid relay pairs)
//
// Each switch has a DIR pin (sets throw direction) and a TRIG pin
// (fires the solenoid pulse). TRIG pins are active LOW:
//   idle = HIGH, pulse = LOW for STAGGER_DELAY_MS, then back to HIGH.
//
// All four switches are triggered together in staggered sequence
// to prevent current/power spikes (QUAD_CONOPS pattern).


struct RelayPair {
  int dirPin;
  int trigPin;
  const char* name;
};

RelayPair switches[] = {
  {4,   6,   "SW1"},
  {20,  7,   "SW2"},
  {21,  12,  "SW3"},
  {5,   A0,  "SW4"}
};

const int NUM_SWITCHES = 4;

// Stagger between consecutive trigger pulses - prevents current spike.
const unsigned long STAGGER_DELAY_MS = 5;

// Direction patterns: element order = {SW1, SW2, SW3, SW4}
// LOW  = default straight position
// HIGH = diverted position
const int PATTERN_OUTER_LOOP[4] = {LOW,  LOW,  LOW,  HIGH};  // outer loop, track 5
const int PATTERN_INNER_LOOP[4] = {HIGH, LOW,  HIGH, LOW};   // inner loop, track 4
const int PATTERN_EXIT_BOARD[4] = {HIGH, LOW,  LOW,  LOW};   // exit board


// SECTION 5: CONOPS TIMING


#define OUTER_PHASE_DURATION_MS 60000UL
#define INNER_PHASE_DURATION_MS 60000UL

bool started       = false;
unsigned long startTime = 0;


// SWITCH FUNCTIONS


// Step 1 of 2: write all DIR pins according to pattern.
void setSwitchDirections(const int pattern[]) {
  for (int i = 0; i < NUM_SWITCHES; i++) {
    digitalWrite(switches[i].dirPin, pattern[i]);
    Serial.print("  [DIR] ");
    Serial.print(switches[i].name);
    Serial.print(" -> ");
    Serial.println(pattern[i] == HIGH ? "HIGH (diverted)" : "LOW (straight)");
  }
}

// Step 2 of 2: fire each TRIG pin with a staggered active-LOW pulse.
// TRIG rests HIGH; pulses LOW for STAGGER_DELAY_MS to actuate solenoid.
void triggerAllSwitchesStaggered() {
  for (int i = 0; i < NUM_SWITCHES; i++) {
    digitalWrite(switches[i].trigPin, LOW);
    delay(STAGGER_DELAY_MS);
    digitalWrite(switches[i].trigPin, HIGH);
    delay(STAGGER_DELAY_MS);
    Serial.print("  [TRIG] ");
    Serial.print(switches[i].name);
    Serial.println(" fired");
  }
}

// Applies a complete switch pattern: set directions then trigger all.
void applySwitchPattern(const int pattern[], const char* label) {
  Serial.println("");
  Serial.print("[SWITCHES] Applying pattern: ");
  Serial.println(label);
  setSwitchDirections(pattern);
  Serial.println("[SWITCHES] Triggering all (staggered active-LOW)...");
  triggerAllSwitchesStaggered();
  Serial.print("[SWITCHES] Pattern complete: ");
  Serial.println(label);
  Serial.println("");
}

// Named switch configurations.
void switchToOuterLoop() { applySwitchPattern(PATTERN_OUTER_LOOP, "OUTER LOOP (track 5)"); }
void switchToInnerLoop() { applySwitchPattern(PATTERN_INNER_LOOP, "INNER LOOP (track 4)"); }
void switchToExit()      { applySwitchPattern(PATTERN_EXIT_BOARD, "EXIT BOARD"); }


// ACTUATION - sensor read, relay safety, servo tick, telemetry
// Called every ~500 ms inside ConOps phase loops.


void runActuation() {
  int outerDist = outerSensor.readRangeContinuousMillimeters();
  int innerDist = innerSensor.readRangeContinuousMillimeters();

  bool new_outer_fallen = (outerDist <= OUTER_FALL_THRESHOLD_MM);
  bool new_inner_fallen = (innerDist <= INNER_FALL_THRESHOLD_MM);

  //  Track power relay: outer (track 5) 
  if (new_outer_fallen != outer_tree_fallen || new_outer_fallen) {
    if (new_outer_fallen) {
      digitalWrite(TRACK_POWER_RELAY_OUTER, HIGH);
      Serial.println("[SAFETY] Outer tree FALLEN - track 5 power CUT");
    } else if (outer_tree_fallen) {
      digitalWrite(TRACK_POWER_RELAY_OUTER, LOW);
      Serial.println("[SAFETY] Outer tree cleared - track 5 power RESTORED");
    }
  }

  //  Track power relay: inner (track 4) 
  if (new_inner_fallen != inner_tree_fallen || new_inner_fallen) {
    if (new_inner_fallen) {
      digitalWrite(TRACK_POWER_RELAY_INNER, HIGH);
      Serial.println("[SAFETY] Inner tree FALLEN - track 4 power CUT");
    } else if (inner_tree_fallen) {
      digitalWrite(TRACK_POWER_RELAY_INNER, LOW);
      Serial.println("[SAFETY] Inner tree cleared - track 4 power RESTORED");
    }
  }

  outer_tree_fallen = new_outer_fallen;
  inner_tree_fallen = new_inner_fallen;

  //  Inner servo tick 
  if (millis() - innerTimer >= innerInterval) {
    innerTimer    = millis();
    innerState    = !innerState;
    innerServo.write(innerState ? 90 : 0);
    inner_tree_lowered = innerState;
    innerInterval = random(5000, 10000);
    Serial.print("[SERVO] Inner tree -> ");
    Serial.println(inner_tree_lowered ? "LOWERED (obstacle possible)" : "RAISED (track clear)");
  }

  //  Outer servo tick 
  if (millis() - outerTimer >= outerInterval) {
    outerTimer    = millis();
    outerState    = !outerState;
    outerServo.write(outerState ? 0 : 90);   // note: outer servo mapping is inverted
    outer_tree_lowered = outerState;
    outerInterval = random(5000, 10000);
    Serial.print("[SERVO] Outer tree -> ");
    Serial.println(outer_tree_lowered ? "LOWERED (obstacle possible)" : "RAISED (track clear)");
  }

  //  Cycle telemetry 
  Serial.print("[SENSOR] outer=");      Serial.print(outerDist);
  Serial.print("mm (");                 Serial.print(outer_tree_fallen ? "FALLEN" : "clear");
  Serial.print(")  inner=");            Serial.print(innerDist);
  Serial.print("mm (");                 Serial.print(inner_tree_fallen ? "FALLEN" : "clear");
  Serial.print(")  | power: track5=");  Serial.print(outer_tree_fallen ? "CUT" : "ON");
  Serial.print("  track4=");            Serial.println(inner_tree_fallen ? "CUT" : "ON");

  delay(500);
}


// SETUP


void setup() {
  Serial.begin(9600);
  delay(3000);
  Serial.println("========================================");
  Serial.println("[INIT] Mt. Rainier Railroad - startup");
  Serial.println("========================================");

  //  Servos 
  Serial.println("[INIT] Attaching servos (both to lowered position)...");
  innerServo.attach(11);
  outerServo.attach(10);
  innerServo.write(90);   // lowered
  outerServo.write(0);    // lowered (outer is inverted)
  Serial.println("[INIT] Servos OK");

  //  VL53L0X sensors: bring up one at a time to assign addresses 
  Serial.println("[INIT] Initializing VL53L0X sensors...");
  pinMode(XSHUT_OUTER, OUTPUT);
  pinMode(XSHUT_INNER, OUTPUT);
  digitalWrite(XSHUT_OUTER, LOW);
  digitalWrite(XSHUT_INNER, LOW);
  delay(500);

  Wire.begin();
  Wire.setClock(400000);

  Serial.println("[INIT] Bringing up outer sensor -> address 0x30...");
  digitalWrite(XSHUT_OUTER, HIGH);
  delay(500);
  outerSensor.init();
  outerSensor.setAddress(0x30);
  outerSensor.startContinuous();
  Serial.println("[INIT] Outer sensor OK (0x30)");

  Serial.println("[INIT] Bringing up inner sensor -> address 0x31...");
  digitalWrite(XSHUT_INNER, HIGH);
  delay(500);
  innerSensor.init();
  innerSensor.setAddress(0x31);
  innerSensor.startContinuous();
  Serial.println("[INIT] Inner sensor OK (0x31)");

  //  Track power relays: both ON at startup 
  Serial.println("[INIT] Track power relays -> both ON (LOW = relay off = power flows)...");
  pinMode(TRACK_POWER_RELAY_INNER, OUTPUT);
  digitalWrite(TRACK_POWER_RELAY_INNER, LOW);
  pinMode(TRACK_POWER_RELAY_OUTER, OUTPUT);
  digitalWrite(TRACK_POWER_RELAY_OUTER, LOW);
  Serial.println("[INIT] Track 4 (inner) power: ON");
  Serial.println("[INIT] Track 5 (outer) power: ON");

  //  Switch pins: DIR LOW, TRIG HIGH (active-LOW idle state) 
  Serial.println("[INIT] Configuring switch pins (TRIG idle = HIGH)...");
  for (int i = 0; i < NUM_SWITCHES; i++) {
    pinMode(switches[i].dirPin,  OUTPUT);
    pinMode(switches[i].trigPin, OUTPUT);
    digitalWrite(switches[i].dirPin,  LOW);
    digitalWrite(switches[i].trigPin, HIGH);
    Serial.print("[INIT]   ");
    Serial.print(switches[i].name);
    Serial.println(" configured");
  }

  //  Throw switches to outer loop starting position 
  Serial.println("[INIT] Throwing switches to initial OUTER LOOP position...");
  switchToOuterLoop();
  Serial.println("[INIT] Switch init complete");

  Serial.println("========================================");
  Serial.println("[INIT] Startup complete - 5s hold before ConOps begins");
  Serial.println("========================================");

  startTime = millis();
}


// LOOP - ConOps
void loop() {


  // HOLD PHASE - 5 seconds after startup before ConOps begins

  if (!started && (millis() - startTime < 5000)) {
    Serial.println("[HOLD] Waiting... (hold phase active)");
    delay(500);
    return;
  }

  if (!started) {
    Serial.println("[CONOPS] Hold phase expired - ConOps starting now");
    innerTimer    = millis();
    outerTimer    = millis();
    innerInterval = random(1000, 5000);
    outerInterval = random(5000, 10000);
    started = true;
  }


  // PHASE 1: OUTER LOOP (60 seconds)
  //
  // Normal: train runs outer loop (track 5).
  // Reroute: if outer track blocked, throw to inner switches
  //          until outer track clears, then restore outer.

  Serial.println("========================================");
  Serial.println("[CONOPS] PHASE 1 START: OUTER LOOP (60 seconds)");
  Serial.println("========================================");
  switchToOuterLoop();

  bool outerRerouted = false;
  unsigned long phaseStart = millis();

  while (millis() - phaseStart < OUTER_PHASE_DURATION_MS) {
    runActuation();

    if (outer_tree_fallen && !outerRerouted) {
      Serial.println("[CONOPS] Outer track blocked - rerouting to INNER LOOP switches");
      switchToInnerLoop();
      outerRerouted = true;
    } else if (!outer_tree_fallen && outerRerouted) {
      Serial.println("[CONOPS] Outer track clear - restoring OUTER LOOP switches");
      switchToOuterLoop();
      outerRerouted = false;
    }
  }

  if (outerRerouted) {
    Serial.println("[CONOPS] Phase 1 ending - restoring OUTER LOOP switches before transition");
    switchToOuterLoop();
  }
  Serial.println("[CONOPS] PHASE 1 COMPLETE");


  // PHASE 2: INNER LOOP (60 seconds)
  //
  // Normal: train runs inner loop (track 4).
  // Reroute: if inner track blocked, throw to outer switches
  //          until inner track clears, then restore inner.

  Serial.println("========================================");
  Serial.println("[CONOPS] PHASE 2 START: INNER LOOP (60 seconds)");
  Serial.println("========================================");
  switchToInnerLoop();

  bool innerRerouted = false;
  phaseStart = millis();

  while (millis() - phaseStart < INNER_PHASE_DURATION_MS) {
    runActuation();

    if (inner_tree_fallen && !innerRerouted) {
      Serial.println("[CONOPS] Inner track blocked - rerouting to OUTER LOOP switches");
      switchToOuterLoop();
      innerRerouted = true;
    } else if (!inner_tree_fallen && innerRerouted) {
      Serial.println("[CONOPS] Inner track clear - restoring INNER LOOP switches");
      switchToInnerLoop();
      innerRerouted = false;
    }
  }

  if (innerRerouted) {
    Serial.println("[CONOPS] Phase 2 ending - restoring INNER LOOP switches before transition");
    switchToInnerLoop();
  }
  Serial.println("[CONOPS] PHASE 2 COMPLETE");


  // PHASE 3: EXIT BOARD (indefinite)
  //
  // Throw switches to exit board and continue actuation forever.
  // Sensors and servos remain active for obstacle safety.

  Serial.println("========================================");
  Serial.println("[CONOPS] PHASE 3 START: EXIT BOARD (indefinite)");
  Serial.println("========================================");
  switchToExit();

  while (true) {
    runActuation();
  }
}
