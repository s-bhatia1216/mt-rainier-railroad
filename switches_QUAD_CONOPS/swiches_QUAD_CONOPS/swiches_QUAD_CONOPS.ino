/////////////////////////////////////////////////////////////////
// ConOps Switch Sequencer
// Outer loop x2 (45s each) → Inner loop x2 (30s each) → Exit
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

const unsigned long OUTER_MS = 10000UL;  // DEBUG: set to 45000 for real run
const unsigned long INNER_MS = 7000UL;   // DEBUG: set to 30000 for real run
const unsigned long PULSE_MS = 20UL;

// =============================================================

void triggerAll() {
  Serial.println("TRIGGERING");
  for (int i = 0; i < NUM_RELAYS; i++) digitalWrite(relays[i].trigPin, LOW);
  delay(PULSE_MS);
  for (int i = 0; i < NUM_RELAYS; i++) digitalWrite(relays[i].trigPin, HIGH);
  delay(PULSE_MS);
}

void setStates(int s[]) {
  for (int i = 0; i < NUM_RELAYS; i++) {
    digitalWrite(relays[i].dirPin, s[i]);
    Serial.print(relays[i].name); Serial.print("="); Serial.println(s[i] ? "HIGH" : "LOW");
  }
}

// =============================================================

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM_RELAYS; i++) {
    pinMode(relays[i].dirPin,  OUTPUT);
    pinMode(relays[i].trigPin, OUTPUT);
    digitalWrite(relays[i].trigPin, HIGH);
  }
  Serial.println("READY");
}

// =============================================================

void loop() {

  // --- OUTER LOOP 1 ---
  Serial.println("=== OUTER 1 ===");
  int outer[] = { LOW, HIGH, LOW, HIGH };
  setStates(outer);
  delay(10000);
  triggerAll();
  delay(OUTER_MS);

  // --- OUTER LOOP 2 ---
  Serial.println("=== OUTER 2 ===");
  setStates(outer);
  delay(10000);
  triggerAll();
  delay(OUTER_MS);

  // --- INNER LOOP 1 ---
  Serial.println("=== INNER 1 ===");
  int inner[] = { HIGH, HIGH, HIGH, LOW };
  setStates(inner);
  delay(10000);
  triggerAll();
  delay(INNER_MS);

  // --- INNER LOOP 2 ---
  Serial.println("=== INNER 2 ===");
  setStates(inner);
  delay(10000);
  triggerAll();
  delay(INNER_MS);

  // --- EXIT ---
  Serial.println("=== EXIT ===");
  int exitPos[] = { HIGH, HIGH, LOW, LOW };
  setStates(exitPos);
  delay(10000);
  triggerAll();

  Serial.println("DONE");
  delay(99999999UL);
}
