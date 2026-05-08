/////////////////////////////////////////////////////////////////
// 4-Relay Simultaneous Test Utility
/////////////////////////////////////////////////////////////////

struct RelayPair {
  int dirPin;
  int trigPin;
  const char* name;
};

// =============================================================
// RELAYS
// =============================================================

RelayPair relays[] = {
  {4,  6,  "SW1"},
  {20, 7,  "SW2"},
  {21, 12, "SW3"},
  {5,  A0, "SW4"}
};

const int NUM_RELAYS = 4;

// =============================================================
// SETTINGS
// =============================================================

const unsigned long MOVE_DELAY = 10000;
const unsigned long PULSE_TIME = 20;

// =============================================================

void triggerAll() {

  Serial.println("TRIGGERING ALL");

  // Pull all trigger pins LOW together
  for (int i = 0; i < NUM_RELAYS; i++) {
    digitalWrite(relays[i].trigPin, LOW);
  }

  delay(PULSE_TIME);

  // Return all trigger pins HIGH together
  for (int i = 0; i < NUM_RELAYS; i++) {
    digitalWrite(relays[i].trigPin, HIGH);
  }

  delay(PULSE_TIME);
}

// =============================================================

void setRelayStates(int states[]) {

  Serial.println("SETTING STATES:");

  for (int i = 0; i < NUM_RELAYS; i++) {

    digitalWrite(relays[i].dirPin, states[i]);

    Serial.print(relays[i].name);
    Serial.print(" = ");

    if (states[i] == HIGH)
      Serial.println("HIGH");
    else
      Serial.println("LOW");
  }
}

// =============================================================

void runPattern(int states[], const char* patternName) {

  Serial.println("================================");
  Serial.println(patternName);

  setRelayStates(states);

  delay(MOVE_DELAY);

  triggerAll();

  Serial.println("PATTERN COMPLETE");
  Serial.println("================================");

  delay(3000);
}

// =============================================================

void setup() {

  Serial.begin(9600);

  for (int i = 0; i < NUM_RELAYS; i++) {

    pinMode(relays[i].dirPin, OUTPUT);
    pinMode(relays[i].trigPin, OUTPUT);

    // idle trigger state
    digitalWrite(relays[i].trigPin, HIGH);
  }

  delay(20);

  Serial.println("4-Relay Simultaneous Test Starting...");
}

// =============================================================

void loop() {

  /////////////////////////////////////////////////////////////
  // PATTERN 1
  // SW1 LOW
  // SW2 LOW
  // SW3 LOW
  // SW4 HIGH
  /////////////////////////////////////////////////////////////

  int pattern1[] = {
    LOW,
    LOW,
    LOW,
    HIGH
  };

  runPattern(pattern1, "PATTERN 1");

  /////////////////////////////////////////////////////////////
  // PATTERN 2
  // SAME AS PATTERN 1
  /////////////////////////////////////////////////////////////

  int pattern2[] = {
    LOW,
    LOW,
    LOW,
    HIGH
  };

  runPattern(pattern2, "PATTERN 2");

  /////////////////////////////////////////////////////////////
  // PATTERN 3
  // SW1 HIGH
  // SW2 LOW
  // SW3 HIGH
  // SW4 HIGH
  /////////////////////////////////////////////////////////////

  int pattern3[] = {
    HIGH,
    LOW,
    HIGH,
    HIGH
  };

  runPattern(pattern3, "PATTERN 3");

  /////////////////////////////////////////////////////////////
  // PATTERN 4
  // SAME AS PATTERN 3
  /////////////////////////////////////////////////////////////

  int pattern4[] = {
    HIGH,
    LOW,
    HIGH,
    HIGH
  };

  runPattern(pattern4, "PATTERN 4");

  /////////////////////////////////////////////////////////////
  // PATTERN 5
  // SW1 HIGH
  // SW2 LOW
  // SW3 HIGH
  // SW4 LOW
  /////////////////////////////////////////////////////////////

  int pattern5[] = {
    HIGH,
    LOW,
    HIGH,
    LOW
  };

  runPattern(pattern5, "PATTERN 5");

  Serial.println("ALL PATTERNS COMPLETE");

  delay(5000);
}