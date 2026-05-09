/////////////////////////////////////////////////////////////////
// Relay / Actuator Test Utility
//
// HOW TO USE:
// 1. Add the DIR/TRIG pin pairs you want to test in testPairs[]
// 2. The script will cycle through each pair automatically
// 3. Each relay moves HIGH, triggers, then LOW, triggers
/////////////////////////////////////////////////////////////////

struct RelayPair {
  int dirPin;
  int trigPin;
  const char* name;
};

// =============================================================
// SELECT WHICH RELAYS TO TEST
// Add or remove entries here
// =============================================================

RelayPair testPairs[] = {
  {4,  6,  "Relay 1"},
  {20, 7,  "Relay 2"},
  {21, 12, "Relay 3"},
  {5,  A0, "Relay 4"}
};

// Number of relay pairs
const int NUM_RELAYS = sizeof(testPairs) / sizeof(testPairs[0]);

// =============================================================
// SETTINGS
// =============================================================

const unsigned long MOVE_DELAY = 10000; // time before trigger
const unsigned long PULSE_TIME = 20;    // trigger pulse width

// =============================================================

void triggerRelay(int trigPin) {
  Serial.println("TRIGGERING LOW");

  digitalWrite(trigPin, LOW);
  delay(PULSE_TIME);

  digitalWrite(trigPin, HIGH);
  delay(PULSE_TIME);
}

void testRelay(RelayPair relay) {

  Serial.println("=================================");
  Serial.print("Testing: ");
  Serial.println(relay.name);

  // Move HIGH
  Serial.println("MOVING TO HIGH");
  digitalWrite(relay.dirPin, HIGH);
  delay(MOVE_DELAY);

  triggerRelay(relay.trigPin);

  // Move LOW
  Serial.println("MOVING TO LOW");
  digitalWrite(relay.dirPin, LOW);
  delay(MOVE_DELAY);

  triggerRelay(relay.trigPin);

  Serial.println("DONE");
  Serial.println("=================================");
  delay(1000);
}

void setup() {

  Serial.begin(9600);

  // Initialize all selected relays
  for (int i = 0; i < NUM_RELAYS; i++) {

    pinMode(testPairs[i].dirPin, OUTPUT);
    pinMode(testPairs[i].trigPin, OUTPUT);

    // idle state
    digitalWrite(testPairs[i].trigPin, HIGH);
  }

  delay(20);

  Serial.println("Relay Test Starting...");
}

void loop() {

  // Test every selected relay
  for (int i = 0; i < NUM_RELAYS; i++) {
    testRelay(testPairs[i]);
  }

  Serial.println("ALL RELAYS TESTED");
  delay(5000);
}