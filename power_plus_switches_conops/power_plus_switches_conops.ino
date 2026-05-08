#include <Wire.h>

#include <VL53L0X.h>

#include <Servo.h>

/////////////////////////////////////////////////////////////

// ---------------- SENSOR SYSTEM ----------------

/////////////////////////////////////////////////////////////

VL53L0X outerSensor;

VL53L0X innerSensor;

#define XSHUT_OUTER 3

#define XSHUT_INNER 2

/////////////////////////////////////////////////////////////

// ---------------- SERVOS ----------------

/////////////////////////////////////////////////////////////

Servo innerServo;

Servo outerServo;

int innerState = 1;

int outerState = 1;

unsigned long innerTimer = 0;

unsigned long outerTimer = 0;

unsigned long innerInterval = 5000;

unsigned long outerInterval = 5000;

bool started = false;

unsigned long startTime = 0;

bool inner_tree_lowered = true;

bool outer_tree_lowered = true;

/////////////////////////////////////////////////////////////

// ---------------- TRACK RELAYS ----------------

/////////////////////////////////////////////////////////////

#define TRACK_POWER_RELAY_1 A1

#define TRACK_POWER_RELAY_2 A2

/////////////////////////////////////////////////////////////

// ---------------- SWITCH SYSTEM ----------------

/////////////////////////////////////////////////////////////

struct RelayPair {

  int dirPin;

  int trigPin;

  const char* name;

};

RelayPair relays[] = {

  {4,  6,  "SW1"},

  {20, 7,  "SW2"},

  {21, 12, "SW3"},

  {5,  A0, "SW4"}

};

const int NUM_RELAYS = 4;

const unsigned long STAGGER_DELAY = 5;

/////////////////////////////////////////////////////////////

// PATTERN DELAYS (UNCHANGED)

/////////////////////////////////////////////////////////////

const unsigned long PATTERN1_DELAY = 30000;

const unsigned long PATTERN2_DELAY = 30000;

const unsigned long PATTERN3_DELAY = 30000;

/////////////////////////////////////////////////////////////

// PATTERNS (STRUCTURE PRESERVED)

/////////////////////////////////////////////////////////////

int pattern1[] = {LOW, LOW, LOW, HIGH};

int pattern2[] = {HIGH, HIGH, HIGH, HIGH};

int pattern3[] = {HIGH, LOW, HIGH, LOW};

/////////////////////////////////////////////////////////////

// STATE MACHINE (HIDDEN FROM USER STRUCTURE)

/////////////////////////////////////////////////////////////

int currentPattern = 0;

unsigned long patternStartTime = 0;

unsigned long currentDelay = 30000;

bool patternActive = false;

bool patternTriggered = false;

/////////////////////////////////////////////////////////////

// LOW LEVEL RELAY FUNCTIONS

/////////////////////////////////////////////////////////////

void triggerAllStaggered() {

  Serial.println("TRIGGERING ALL");

  for (int i = 0; i < NUM_RELAYS; i++) {

    digitalWrite(relays[i].trigPin, LOW);

    delay(STAGGER_DELAY);

    digitalWrite(relays[i].trigPin, HIGH);

    delay(STAGGER_DELAY);

  }

}

void setRelayStates(int states[]) {

  Serial.println("SETTING STATES");

  for (int i = 0; i < NUM_RELAYS; i++) {

    digitalWrite(relays[i].dirPin, states[i]);

    Serial.print(relays[i].name);

    Serial.print(" = ");

    Serial.println(states[i] == HIGH ? "HIGH" : "LOW");

  }

}

void runPattern(int states[], const char* name, unsigned long delayTime) {

  // start pattern once

  if (!patternTriggered) {

    Serial.println("================================");

    Serial.println(name);

    setRelayStates(states);

    triggerAllStaggered();

    patternStartTime = millis();

    currentDelay = delayTime;

    patternTriggered = true;

    Serial.println("DONE");

    Serial.println("================================");

  }

  // wait NON-BLOCKING

  if (millis() - patternStartTime >= currentDelay) {

    currentPattern++;

    patternTriggered = false;

  }

}

/////////////////////////////////////////////////////////////

// SETUP

/////////////////////////////////////////////////////////////

void setup() {

  Serial.begin(9600);

  delay(3000);

  Serial.println(F("A: START"));

  innerServo.attach(11);

  outerServo.attach(10);

  innerServo.write(90);

  outerServo.write(0);

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

  pinMode(TRACK_POWER_RELAY_1, OUTPUT);

  pinMode(TRACK_POWER_RELAY_2, OUTPUT);

  digitalWrite(TRACK_POWER_RELAY_1, LOW);

  digitalWrite(TRACK_POWER_RELAY_2, LOW);

  for (int i = 0; i < NUM_RELAYS; i++) {

    pinMode(relays[i].dirPin, OUTPUT);

    pinMode(relays[i].trigPin, OUTPUT);

    digitalWrite(relays[i].trigPin, HIGH);

  }

  startTime = millis();

}

/////////////////////////////////////////////////////////////

// LOOP (STRUCTURE PRESERVED EXACTLY)

/////////////////////////////////////////////////////////////

void loop() {

  /////////////////////////////////////////////////////////////

  // HOLD PHASE

  /////////////////////////////////////////////////////////////

  if (!started && (millis() - startTime < 5000)) {

    Serial.println(F("hold_phase=1"));

    return;

  }

  if (!started) {

    innerTimer = millis();

    outerTimer = millis();

    innerInterval = random(1000, 5000);

    outerInterval = random(5000, 10000);

    started = true;

  }

  /////////////////////////////////////////////////////////////

  // SENSOR READ

  /////////////////////////////////////////////////////////////

  int outerDist = outerSensor.readRangeContinuousMillimeters();

  int innerDist = innerSensor.readRangeContinuousMillimeters();

  bool outer_tree_fallen = (outerDist <= 170);

  bool inner_tree_fallen = (innerDist <= 120);

  /////////////////////////////////////////////////////////////

  // TRACK SAFETY

  /////////////////////////////////////////////////////////////

  digitalWrite(TRACK_POWER_RELAY_1, inner_tree_fallen ? HIGH : LOW);

  digitalWrite(TRACK_POWER_RELAY_2, outer_tree_fallen ? HIGH : LOW);

  /////////////////////////////////////////////////////////////

  // SERVO LOGIC

  /////////////////////////////////////////////////////////////

  if (millis() - innerTimer >= innerInterval) {

    innerTimer = millis();

    innerState = !innerState;

    innerServo.write(innerState ? 90 : 0);

    innerInterval = random(5000, 10000);

  }

  if (millis() - outerTimer >= outerInterval) {

    outerTimer = millis();

    outerState = !outerState;

    outerServo.write(outerState ? 0 : 90);

    outerInterval = random(5000, 10000);

  }

  /////////////////////////////////////////////////////////////

  // SWITCH SYSTEM (LOOKS IDENTICAL TO YOUR ORIGINAL STYLE)

  /////////////////////////////////////////////////////////////

  if (currentPattern == 0) {

    /////////////////////////////////////////////////////////////

    // PATTERN 1: outer loop

    /////////////////////////////////////////////////////////////

    runPattern(pattern1, "PATTERN 1", PATTERN1_DELAY);

  }

  else if (currentPattern == 1) {

    /////////////////////////////////////////////////////////////

    // PATTERN 2: inner loop

    /////////////////////////////////////////////////////////////

    runPattern(pattern2, "PATTERN 2", PATTERN2_DELAY);

  }

  else if (currentPattern == 2) {

    /////////////////////////////////////////////////////////////

    // PATTERN 3... exit

    /////////////////////////////////////////////////////////////

    runPattern(pattern3, "PATTERN 3", PATTERN3_DELAY);

  }

  else {

    currentPattern = 0;

  }

  /////////////////////////////////////////////////////////////

  // TELEMETRY

  /////////////////////////////////////////////////////////////

  Serial.print("inner_dist="); Serial.print(innerDist);

  Serial.print(" outer_dist="); Serial.print(outerDist);

  Serial.print(" | INNER_FALLEN="); Serial.print(inner_tree_fallen);

  Serial.print(" OUTER_FALLEN="); Serial.println(outer_tree_fallen);

  delay(500);

}