// Mt. Rainier Railroad - Integrated Master Controller

#include <Wire.h>
#include <VL53L0X.h>
#include <Servo.h>

// ---- SENSORS ----
VL53L0X outerSensor;
VL53L0X innerSensor;

#define XSHUT_OUTER 3
#define XSHUT_INNER 2
#define OUTER_FALL_THRESHOLD_MM 170
#define INNER_FALL_THRESHOLD_MM 120

bool outer_tree_fallen = false;
bool inner_tree_fallen = false;

// ---- SERVOS ----
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

// ---- TRACK POWER RELAYS ----
// LOW = relay off = power ON | HIGH = relay on = power CUT
#define TRACK_POWER_RELAY_INNER A1
#define TRACK_POWER_RELAY_OUTER A2

// ---- TRACK SWITCHES ----
// TRIG pins active LOW: idle HIGH, pulse LOW to actuate solenoid.
struct RelayPair { int dirPin; int trigPin; };

RelayPair switches[] = {
  {4,  6 },   // SW1
  {20, 7 },   // SW2
  {21, 12},   // SW3
  {5,  A0}    // SW4
};

const int NUM_SWITCHES = 4;
const unsigned long STAGGER_DELAY_MS = 5;

// Patterns: {SW1, SW2, SW3, SW4} — LOW=straight, HIGH=diverted
const int PATTERN_OUTER[4] = {LOW,  LOW,  LOW,  HIGH};
const int PATTERN_INNER[4] = {HIGH, LOW,  HIGH, LOW };
const int PATTERN_EXIT[4]  = {HIGH, LOW,  LOW,  LOW };

// ---- CONOPS TIMING ----
#define OUTER_PHASE_MS 60000UL
#define INNER_PHASE_MS 60000UL

bool started = false;
unsigned long startTime = 0;

////////////////////////////////////////////////////////////////
// SWITCH FUNCTIONS
////////////////////////////////////////////////////////////////

void setSwitchDirections(const int pattern[]) {
  for (int i = 0; i < NUM_SWITCHES; i++)
    digitalWrite(switches[i].dirPin, pattern[i]);
}

void triggerAllSwitchesStaggered() {
  for (int i = 0; i < NUM_SWITCHES; i++) {
    digitalWrite(switches[i].trigPin, LOW);
    delay(STAGGER_DELAY_MS);
    digitalWrite(switches[i].trigPin, HIGH);
    delay(STAGGER_DELAY_MS);
  }
}

void applySwitchPattern(const int pattern[], const __FlashStringHelper* label) {
  Serial.print(F("[SWITCHES] -> ")); Serial.println(label);
  setSwitchDirections(pattern);
  triggerAllSwitchesStaggered();
  Serial.print(F("[SWITCHES] done: ")); Serial.println(label);
}

void switchToOuterLoop() { applySwitchPattern(PATTERN_OUTER, F("OUTER LOOP")); }
void switchToInnerLoop() { applySwitchPattern(PATTERN_INNER, F("INNER LOOP")); }
void switchToExit()      { applySwitchPattern(PATTERN_EXIT,  F("EXIT BOARD")); }

////////////////////////////////////////////////////////////////
// ACTUATION — sensor read, relay safety, servo tick, telemetry
////////////////////////////////////////////////////////////////

void runActuation() {
  int outerDist = outerSensor.readRangeContinuousMillimeters();
  int innerDist = innerSensor.readRangeContinuousMillimeters();

  bool new_outer = (outerDist <= OUTER_FALL_THRESHOLD_MM);
  bool new_inner = (innerDist <= INNER_FALL_THRESHOLD_MM);

  // Outer track power relay (track 5)
  if (new_outer != outer_tree_fallen || new_outer) {
    digitalWrite(TRACK_POWER_RELAY_OUTER, new_outer ? HIGH : LOW);
    if (new_outer)          Serial.println(F("[SAFETY] Outer FALLEN - track5 CUT"));
    else if (outer_tree_fallen) Serial.println(F("[SAFETY] Outer clear - track5 ON"));
  }

  // Inner track power relay (track 4)
  if (new_inner != inner_tree_fallen || new_inner) {
    digitalWrite(TRACK_POWER_RELAY_INNER, new_inner ? HIGH : LOW);
    if (new_inner)          Serial.println(F("[SAFETY] Inner FALLEN - track4 CUT"));
    else if (inner_tree_fallen) Serial.println(F("[SAFETY] Inner clear - track4 ON"));
  }

  outer_tree_fallen = new_outer;
  inner_tree_fallen = new_inner;

  // Inner servo tick
  if (millis() - innerTimer >= innerInterval) {
    innerTimer    = millis();
    innerState    = !innerState;
    innerServo.write(innerState ? 90 : 0);
    inner_tree_lowered = innerState;
    innerInterval = random(5000, 10000);
    Serial.print(F("[SERVO] Inner -> "));
    Serial.println(inner_tree_lowered ? F("LOWERED") : F("RAISED"));
  }

  // Outer servo tick
  if (millis() - outerTimer >= outerInterval) {
    outerTimer    = millis();
    outerState    = !outerState;
    outerServo.write(outerState ? 0 : 90);   // inverted mapping
    outer_tree_lowered = outerState;
    outerInterval = random(5000, 10000);
    Serial.print(F("[SERVO] Outer -> "));
    Serial.println(outer_tree_lowered ? F("LOWERED") : F("RAISED"));
  }

  // Telemetry
  Serial.print(F("[SENSOR] out=")); Serial.print(outerDist);
  Serial.print(F("mm ")); Serial.print(outer_tree_fallen ? F("FALLEN") : F("clear"));
  Serial.print(F(" in="));  Serial.print(innerDist);
  Serial.print(F("mm ")); Serial.print(inner_tree_fallen ? F("FALLEN") : F("clear"));
  Serial.print(F(" | trk5=")); Serial.print(outer_tree_fallen ? F("CUT") : F("ON"));
  Serial.print(F(" trk4="));   Serial.println(inner_tree_fallen ? F("CUT") : F("ON"));

  delay(500);
}

////////////////////////////////////////////////////////////////
// SETUP
////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  delay(3000);
  Serial.println(F("[INIT] startup"));

  // Servos
  innerServo.attach(11);
  outerServo.attach(10);
  innerServo.write(90);
  outerServo.write(0);
  Serial.println(F("[INIT] servos OK"));

  // Sensors
  pinMode(XSHUT_OUTER, OUTPUT);
  pinMode(XSHUT_INNER, OUTPUT);
  digitalWrite(XSHUT_OUTER, LOW);
  digitalWrite(XSHUT_INNER, LOW);
  delay(500);
  Wire.begin();
  Wire.setClock(400000);

  Serial.println(F("[INIT] outer sensor 0x30..."));
  digitalWrite(XSHUT_OUTER, HIGH); delay(500);
  outerSensor.init();
  outerSensor.setAddress(0x30);
  outerSensor.startContinuous();
  Serial.println(F("[INIT] outer sensor OK"));

  Serial.println(F("[INIT] inner sensor 0x31..."));
  digitalWrite(XSHUT_INNER, HIGH); delay(500);
  innerSensor.init();
  innerSensor.setAddress(0x31);
  innerSensor.startContinuous();
  Serial.println(F("[INIT] inner sensor OK"));

  // Track power relays — both ON
  pinMode(TRACK_POWER_RELAY_INNER, OUTPUT); digitalWrite(TRACK_POWER_RELAY_INNER, LOW);
  pinMode(TRACK_POWER_RELAY_OUTER, OUTPUT); digitalWrite(TRACK_POWER_RELAY_OUTER, LOW);
  Serial.println(F("[INIT] relays ON (track4 + track5)"));

  // Switch pins — TRIG idle HIGH (active LOW)
  for (int i = 0; i < NUM_SWITCHES; i++) {
    pinMode(switches[i].dirPin,  OUTPUT); digitalWrite(switches[i].dirPin,  LOW);
    pinMode(switches[i].trigPin, OUTPUT); digitalWrite(switches[i].trigPin, HIGH);
  }
  Serial.println(F("[INIT] switch pins configured"));

  switchToOuterLoop();
  Serial.println(F("[INIT] switches -> OUTER LOOP"));
  Serial.println(F("[INIT] holding 5s before ConOps..."));

  startTime = millis();
}

////////////////////////////////////////////////////////////////
// LOOP — ConOps
////////////////////////////////////////////////////////////////

void loop() {

  // Hold phase
  if (!started && (millis() - startTime < 5000)) {
    Serial.println(F("[HOLD] waiting..."));
    delay(500);
    return;
  }

  if (!started) {
    Serial.println(F("[CONOPS] hold done — starting"));
    innerTimer    = millis();
    outerTimer    = millis();
    innerInterval = random(1000, 5000);
    outerInterval = random(5000, 10000);
    started = true;
  }

  // ---- PHASE 1: OUTER LOOP (60s) ----
  Serial.println(F("[CONOPS] PHASE 1: OUTER LOOP (60s)"));
  switchToOuterLoop();
  bool outerRerouted = false;
  unsigned long phaseStart = millis();

  while (millis() - phaseStart < OUTER_PHASE_MS) {
    runActuation();
    if (outer_tree_fallen && !outerRerouted) {
      Serial.println(F("[CONOPS] outer blocked -> rerouting INNER"));
      switchToInnerLoop();
      outerRerouted = true;
    } else if (!outer_tree_fallen && outerRerouted) {
      Serial.println(F("[CONOPS] outer clear -> restoring OUTER"));
      switchToOuterLoop();
      outerRerouted = false;
    }
  }
  if (outerRerouted) { Serial.println(F("[CONOPS] phase1 end -> restore OUTER")); switchToOuterLoop(); }
  Serial.println(F("[CONOPS] PHASE 1 done"));

  // ---- PHASE 2: INNER LOOP (60s) ----
  Serial.println(F("[CONOPS] PHASE 2: INNER LOOP (60s)"));
  switchToInnerLoop();
  bool innerRerouted = false;
  phaseStart = millis();

  while (millis() - phaseStart < INNER_PHASE_MS) {
    runActuation();
    if (inner_tree_fallen && !innerRerouted) {
      Serial.println(F("[CONOPS] inner blocked -> rerouting OUTER"));
      switchToOuterLoop();
      innerRerouted = true;
    } else if (!inner_tree_fallen && innerRerouted) {
      Serial.println(F("[CONOPS] inner clear -> restoring INNER"));
      switchToInnerLoop();
      innerRerouted = false;
    }
  }
  if (innerRerouted) { Serial.println(F("[CONOPS] phase2 end -> restore INNER")); switchToInnerLoop(); }
  Serial.println(F("[CONOPS] PHASE 2 done"));

  // ---- PHASE 3: EXIT (indefinite) ----
  Serial.println(F("[CONOPS] PHASE 3: EXIT BOARD (indefinite)"));
  switchToExit();
  while (true) { runActuation(); }
}
