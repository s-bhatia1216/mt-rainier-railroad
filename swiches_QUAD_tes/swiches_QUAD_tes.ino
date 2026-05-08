/////////////////////////////////////////////////////////////////
// 4-Relay Simultaneous Test Utility
//
// Example pattern:
//   SW1 = LOW
//   SW2 = LOW
//   SW3 = LOW
//   SW4 = HIGH
//
// Then trigger ALL relays together
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
// Set all relay directions at once
// states[] contains HIGH or LOW for each switch
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
    LOW,   // SW1
    LOW,   // SW2
    LOW,   // SW3
    HIGH   // SW4
  };

  setRelayStates(pattern1);

  delay(MOVE_DELAY);

  triggerAll();

  Serial.println("PATTERN 1 COMPLETE");
  Serial.println("--------------------------------");

  delay(3000);

  /////////////////////////////////////////////////////////////
  // PATTERN 2
  // Example reverse pattern
  /////////////////////////////////////////////////////////////

  int pattern2[] = {
    HIGH,
    HIGH,
    HIGH,
    LOW
  };

  setRelayStates(pattern2);

  delay(MOVE_DELAY);

  triggerAll();

  Serial.println("PATTERN 2 COMPLETE");
  Serial.println("--------------------------------");

  delay(5000);
}