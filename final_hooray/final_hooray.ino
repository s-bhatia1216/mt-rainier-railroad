#include <SoftwareSerial.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Servo.h>

/////////////////////////////////////////////////////////////
// ---------------- SOFTWARE SERIAL ----------------
/////////////////////////////////////////////////////////////

SoftwareSerial SoftSerial(8, 9);

byte a = 0x00;
byte ap = 0x00; // Stores the previous value (0x## is Hexadecimal)
byte nmask = 0b00001111; // Strips the high nibble (STrain)
byte smask = 0b11110000; // Strips the low nibble (NTrain)
byte ntrain = 0;
byte strain = 0;
byte ntrainp = 0; // Hold a history of the last value
byte strainp = 0; 

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

bool switchingActive = false;

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
// PATTERN DELAYS
/////////////////////////////////////////////////////////////

const unsigned long PATTERN1_DELAY = 60000;
const unsigned long PATTERN2_DELAY = 30000;
const unsigned long PATTERN3_DELAY = 30000;

/////////////////////////////////////////////////////////////
// PATTERNS
/////////////////////////////////////////////////////////////

int pattern1[] = {LOW, LOW, LOW, HIGH};
int pattern2[] = {HIGH, HIGH, HIGH, HIGH};
int pattern3[] = {HIGH, LOW, HIGH, LOW};

/////////////////////////////////////////////////////////////
// STATE MACHINE
/////////////////////////////////////////////////////////////

int currentPattern = 0;

unsigned long patternStartTime = 0;
unsigned long currentDelay = 30000;

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

  if (millis() - patternStartTime >= currentDelay) {

    currentPattern++;

    patternTriggered = false;
  }
}

/////////////////////////////////////////////////////////////
// SLOW SERVO FUNCTION
/////////////////////////////////////////////////////////////

void moveServoSlow(Servo &servo, int startPos, int endPos, int stepDelay) {

  if (startPos < endPos) {

    for (int pos = startPos; pos <= endPos; pos++) {

      servo.write(pos);

      delay(stepDelay);
    }

  } else {

    for (int pos = startPos; pos >= endPos; pos--) {

      servo.write(pos);

      delay(stepDelay);
    }
  }
}

/////////////////////////////////////////////////////////////
// TRAIN FUNCTION
/////////////////////////////////////////////////////////////

void function1() {

  Serial.println("<t 3 50 1>"); // cab speed dir, speed 0..127 or -1 Estop, dir 1=forward 0=reverse  
  delay(5000);

  //Serial.println("<0>");
}

/////////////////////////////////////////////////////////////
// SETUP
/////////////////////////////////////////////////////////////

void setup() {

  Serial.begin(9600);

  SoftSerial.begin(9600);

  Serial.println("<1>"); // Turns on track power!

  delay(3000);

  Serial.println(F("A: START"));

  /////////////////////////////////////////////////////////////
  // SERVOS
  /////////////////////////////////////////////////////////////

  innerServo.attach(11);
  outerServo.attach(10);

  innerServo.write(90);
  outerServo.write(0);

  /////////////////////////////////////////////////////////////
  // VL53L0X
  /////////////////////////////////////////////////////////////

  pinMode(XSHUT_OUTER, OUTPUT);
  pinMode(XSHUT_INNER, OUTPUT);

  digitalWrite(XSHUT_OUTER, LOW);
  digitalWrite(XSHUT_INNER, LOW);

  delay(500);

  Wire.begin();

  Wire.setClock(400000);

  digitalWrite(XSHUT_OUTER, HIGH);
  delay(500);

  outerSensor.init();
  outerSensor.setAddress(0x30);
  outerSensor.startContinuous();

  digitalWrite(XSHUT_INNER, HIGH);
  delay(500);

  innerSensor.init();
  innerSensor.setAddress(0x31);
  innerSensor.startContinuous();

  Serial.println(F("F: BOTH SYSTEMS RUNNING"));

  /////////////////////////////////////////////////////////////
  // TRACK RELAYS
  /////////////////////////////////////////////////////////////

  pinMode(TRACK_POWER_RELAY_1, OUTPUT);
  pinMode(TRACK_POWER_RELAY_2, OUTPUT);

  digitalWrite(TRACK_POWER_RELAY_1, LOW);
  digitalWrite(TRACK_POWER_RELAY_2, LOW);

  /////////////////////////////////////////////////////////////
  // SWITCH RELAYS
  /////////////////////////////////////////////////////////////

  for (int i = 0; i < NUM_RELAYS; i++) {

    pinMode(relays[i].dirPin, OUTPUT);
    pinMode(relays[i].trigPin, OUTPUT);

    digitalWrite(relays[i].trigPin, HIGH);
  }

  startTime = millis();
}

/////////////////////////////////////////////////////////////
// LOOP
/////////////////////////////////////////////////////////////

void loop() {

  /////////////////////////////////////////////////////////////
  // TRAIN DETECTION
  /////////////////////////////////////////////////////////////

  byte c = 0x31; // Any Byte triggers a reply from SBC with the byte containing STrain and NTrain

  delay(190); // A delay for testing

  SoftSerial.write(c); // Send a byte to the ACIA to trigger it to query the signal sensor board and return STrain|Ntrain in a byte
  SoftSerial.flush(); // Waits for above Tx to finish sending before proceeding
  delay(10); // Waits a short while to allow the next step to process the returned byte, might not be necessary with flush()

  if (SoftSerial.available() > 0) {  // Checks if ACIA sent a byte
    a = SoftSerial.read(); // Get the byte from ACIA
    if (a != ap){  // Checks if the byte received is different from the last one
      ntrain = a & nmask;
      strain = (a & smask) >> 4;
      if (ntrain != ntrainp){
        // Code here runs when NTrain has changed
        if (ntrain != 0){ // Tests for when North block gets occupied (NTrain NOT zero)
          switchingActive = true;
          function1();
        }
      }
      if (strain != strainp){
        // Put any code you want when STrain has changed

      }
      ntrainp = ntrain;
      strainp = strain;
      ap = a; //Update the record for next loop
    }
  }

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

  digitalWrite(TRACK_POWER_RELAY_1,
               inner_tree_fallen ? HIGH : LOW);

  digitalWrite(TRACK_POWER_RELAY_2,
               outer_tree_fallen ? HIGH : LOW);

  if (inner_tree_fallen && digitalRead(TRACK_POWER_RELAY_1) == HIGH &&
      outer_tree_fallen && digitalRead(TRACK_POWER_RELAY_2) == HIGH) {

      Serial.println("BOTH TRACK 4 AND TRACK 5 HAVE BEEN CUT");

  }
  else if (inner_tree_fallen && digitalRead(TRACK_POWER_RELAY_1) == HIGH) {

      Serial.println("TRACK 4 CUT - INNER OBSTACLE DETECTED");

  }
  else if (outer_tree_fallen && digitalRead(TRACK_POWER_RELAY_2) == HIGH) {

      Serial.println("TRACK 5 CUT - OUTER OBSTACLE DETECTED");

  }

  /////////////////////////////////////////////////////////////
  // SERVO LOGIC
  /////////////////////////////////////////////////////////////

  if (millis() - innerTimer >= innerInterval) {

    innerTimer = millis();

    innerState = !innerState;

    //innerServo.write(innerState ? 90 : 0);

    moveServoSlow(
      innerServo,
      innerState ? 0 : 110,
      innerState ? 110 : 0,
      15
    );

    innerInterval = random(5000, 10000);
  }

  if (millis() - outerTimer >= outerInterval) {

    outerTimer = millis();

    outerState = !outerState;

    //outerServo.write(outerState ? 0 : 90);

    moveServoSlow(
      outerServo,
      outerState ? 110 : 0,
      outerState ? 0 : 110,
      15
    );

    outerInterval = random(5000, 10000);
  }

  /////////////////////////////////////////////////////////////
  // SWITCH SYSTEM
  /////////////////////////////////////////////////////////////

  if (switchingActive) {

    if (currentPattern == 0) {

      runPattern(pattern1,
                 "PATTERN 1",
                 PATTERN1_DELAY);
    }

    else if (currentPattern == 1) {

      runPattern(pattern2,
                 "PATTERN 2",
                 PATTERN2_DELAY);
    }

    else if (currentPattern == 2) {

      runPattern(pattern3,
                 "PATTERN 3",
                 PATTERN3_DELAY);
    }

    else {

      currentPattern = 0;
    }
  }

  /////////////////////////////////////////////////////////////
  // TELEMETRY
  /////////////////////////////////////////////////////////////

  Serial.print("PATTERN ");
  Serial.print(currentPattern + 1);

  Serial.print(" | ");

  Serial.print("inner_dist=");
  Serial.print(innerDist);

  Serial.print(" outer_dist=");
  Serial.print(outerDist);

  Serial.print(" | INNER_FALLEN=");
  Serial.print(inner_tree_fallen);

  Serial.print(" OUTER_FALLEN=");
  Serial.println(outer_tree_fallen);

  delay(500);
}