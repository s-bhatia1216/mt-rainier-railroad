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

/////////////////////////////////////////////////////////////////
// TIMING SETTINGS
/////////////////////////////////////////////////////////////////

// Time AFTER setting directions BEFORE triggering relays
const unsigned long MOVE_DELAY = 10000;

// Tiny spacing between each relay trigger pulse
// prevents power/current spike
const unsigned long STAGGER_DELAY = 5;

// Custom delay AFTER each pattern finishes
const unsigned long PATTERN1_DELAY = 30000;
const unsigned long PATTERN2_DELAY = 25000;
const unsigned long PATTERN3_DELAY = 10000;

/////////////////////////////////////////////////////////////////

void triggerAllStaggered() {

  Serial.println("TRIGGERING ALL");

  for (int i = 0; i < NUM_RELAYS; i++) {

    digitalWrite(relays[i].trigPin, LOW);
    delay(STAGGER_DELAY);

    digitalWrite(relays[i].trigPin, HIGH);
    delay(STAGGER_DELAY);
  }
}

/////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////

void runPattern(
  int states[],
  const char* patternName,
  unsigned long patternDelay
) {

  Serial.println("================================");
  Serial.println(patternName);

  // Set relay directions
  setRelayStates(states);

  // Wait before triggering
  delay(MOVE_DELAY);

  // Trigger relays
  triggerAllStaggered();

  Serial.println("DONE");
  Serial.println("================================");

  // Wait after pattern completes
  delay(patternDelay);
}

/////////////////////////////////////////////////////////////////

void setup() {

  Serial.begin(9600);

  for (int i = 0; i < NUM_RELAYS; i++) {

    pinMode(relays[i].dirPin, OUTPUT);
    pinMode(relays[i].trigPin, OUTPUT);

    digitalWrite(relays[i].trigPin, HIGH);
  }

  Serial.println("STARTING...");
}

/////////////////////////////////////////////////////////////////

void loop() {

  /////////////////////////////////////////////////////////////
  // PATTERN 1: outer loop
  /////////////////////////////////////////////////////////////

  int pattern1[] = {
    LOW,
    LOW,
    LOW,
    HIGH
  };

  runPattern(
    pattern1,
    "PATTERN 1",
    PATTERN1_DELAY
  );

  /////////////////////////////////////////////////////////////
  // PATTERN 2: inner loop
  /////////////////////////////////////////////////////////////

  int pattern2[] = {
    HIGH,
    HIGH,
    HIGH,
    HIGH
  };

  runPattern(
    pattern2,
    "PATTERN 2",
    PATTERN2_DELAY
  );

  /////////////////////////////////////////////////////////////
  // PATTERN 3... exit 
  /////////////////////////////////////////////////////////////

  int pattern3[] = {
    HIGH,
    LOW,
    HIGH,
    LOW
  };

  runPattern(
    pattern3,
    "PATTERN 3",
    PATTERN3_DELAY
  );
}