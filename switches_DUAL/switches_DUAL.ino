/////////////////////////////////////////////////////////////////
// Relay / Actuator Group Test Utility
//
// GROUP 1:
//   Relay 1 + Relay 2 together
//
// GROUP 2:
//   Relay 3 + Relay 4 together
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
  {4,  6,  "Relay 1"},
  {20, 7,  "Relay 2"},
  {21, 12, "Relay 3"},
  {5,  A0, "Relay 4"}
};

// =============================================================
// SETTINGS
// =============================================================

const unsigned long MOVE_DELAY = 10000;
const unsigned long PULSE_TIME = 20;

// =============================================================

void triggerRelay(int trigPin) {

  digitalWrite(trigPin, LOW);
  delay(PULSE_TIME);

  digitalWrite(trigPin, HIGH);
  delay(PULSE_TIME);
}

// Trigger multiple relays together
void triggerGroup(RelayPair group[], int count) {

  Serial.println("TRIGGERING GROUP");

  // Pull all LOW together
  for (int i = 0; i < count; i++) {
    digitalWrite(group[i].trigPin, LOW);
  }

  delay(PULSE_TIME);

  // Return all HIGH together
  for (int i = 0; i < count; i++) {
    digitalWrite(group[i].trigPin, HIGH);
  }

  delay(PULSE_TIME);
}

// Test a group simultaneously
void testGroup(RelayPair group[], int count, const char* groupName) {

  Serial.println("=================================");
  Serial.print("Testing ");
  Serial.println(groupName);

  // -------------------------------------------------
  // MOVE HIGH
  // -------------------------------------------------

  Serial.println("MOVING GROUP TO HIGH");

  for (int i = 0; i < count; i++) {
    digitalWrite(group[i].dirPin, HIGH);
  }

  delay(MOVE_DELAY);

  triggerGroup(group, count);

  // -------------------------------------------------
  // MOVE LOW
  // -------------------------------------------------

  Serial.println("MOVING GROUP TO LOW");

  for (int i = 0; i < count; i++) {
    digitalWrite(group[i].dirPin, LOW);
  }

  delay(MOVE_DELAY);

  triggerGroup(group, count);

  Serial.println("GROUP DONE");
  Serial.println("=================================");

  delay(1000);
}

void setup() {

  Serial.begin(9600);

  // Initialize all relays
  for (int i = 0; i < 4; i++) {

    pinMode(relays[i].dirPin, OUTPUT);
    pinMode(relays[i].trigPin, OUTPUT);

    digitalWrite(relays[i].trigPin, HIGH);
  }

  delay(20);

  Serial.println("Relay Group Test Starting...");
}

void loop() {

  // =================================================
  // GROUP 1 -> Relay 1 + Relay 2
  // =================================================

  RelayPair group1[] = {
    relays[0],
    relays[1]
  };

  testGroup(group1, 2, "GROUP 1");

  // =================================================
  // GROUP 2 -> Relay 3 + Relay 4
  // =================================================

  RelayPair group2[] = {
    relays[2],
    relays[3]
  };

  testGroup(group2, 2, "GROUP 2");

  Serial.println("ALL GROUPS TESTED");

  delay(5000);
}