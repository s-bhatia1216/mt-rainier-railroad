/////////////////////////////////////////////////////////////////
// 4-Relay Coordinated Trigger Test
/////////////////////////////////////////////////////////////////

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

const unsigned long MOVE_DELAY = 10000;
const unsigned long PULSE_TIME = 50;

// =============================================================

void triggerAllStaggered() {

  Serial.println("TRIGGERING ALL");

  // Trigger one-by-one VERY quickly
  // prevents power collapse/current spike

  for (int i = 0; i < NUM_RELAYS; i++) {

    digitalWrite(relays[i].trigPin, LOW);
    delay(5);

    digitalWrite(relays[i].trigPin, HIGH);
    delay(5);
  }
}

// =============================================================

void setRelayStates(int states[]) {

  Serial.println("SETTING STATES");

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

  // Set all directions first
  setRelayStates(states);

  // Wait for actuator direction setup
  delay(MOVE_DELAY);

  // Trigger all (slightly staggered)
  triggerAllStaggered();

  Serial.println("DONE");
  Serial.println("================================");

  delay(3000);
}

// =============================================================

void setup() {

  Serial.begin(9600);

  for (int i = 0; i < NUM_RELAYS; i++) {

    pinMode(relays[i].dirPin, OUTPUT);
    pinMode(relays[i].trigPin, OUTPUT);

    digitalWrite(relays[i].trigPin, HIGH);
  }

  Serial.println("STARTING...");
}

// =============================================================

void loop() {

  /////////////////////////////////////////////////////////////
  // PATTERN 1
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
  /////////////////////////////////////////////////////////////

  int pattern2[] = {
    HIGH,
    LOW,
    HIGH,
    HIGH
  };

  runPattern(pattern2, "PATTERN 2");

  /////////////////////////////////////////////////////////////
  // PATTERN 3
  /////////////////////////////////////////////////////////////

  int pattern3[] = {
    HIGH,
    LOW,
    HIGH,
    LOW
  };

  runPattern(pattern3, "PATTERN 3");

  delay(5000);
}