// // DIR pins 
// const int DIR1 = 4;
// const int DIR2 = 20;
// const int DIR3 = 21;
// const int DIR4 = 5;

// // TRIG pins 
// const int TRIG1 = 6;
// const int TRIG2 = 7;
// const int TRIG3 = 12;
// const int TRIG4 = A0;

// void setup() {
//   pinMode(DIR1, OUTPUT);
//   pinMode(DIR2, OUTPUT);
//   pinMode(DIR3, OUTPUT);
//   pinMode(DIR4, OUTPUT);

//   pinMode(TRIG1, OUTPUT);
//   pinMode(TRIG2, OUTPUT);
//   pinMode(TRIG3, OUTPUT);
//   pinMode(TRIG4, OUTPUT);

//   digitalWrite(TRIG1, LOW);
//   digitalWrite(TRIG2, LOW);
//   digitalWrite(TRIG3, LOW);
//   digitalWrite(TRIG4, LOW);
// }

// // basic pulse function
// void pulse(int trigPin) {
//   digitalWrite(trigPin, HIGH);
//   delay(120);
//   digitalWrite(trigPin, LOW);
// }

// // switch control (DIR + TRIG together)
// void moveSwitch(int dirPin, int trigPin, bool dir) {
//   digitalWrite(dirPin, dir);
//   delay(2);
//   pulse(trigPin);
// }

// void loop() {
//   moveSwitch(DIR1, TRIG1, true);
//   delay(2000);

//   moveSwitch(DIR1, TRIG1, false);
//   delay(2000);
// }







///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ===== SELECT WHICH RELAY TO TEST =====

// Relay 1
// const int DIR_PIN  = 4;
// const int TRIG_PIN = 6;

// Relay 2
const int DIR_PIN  = 20;
const int TRIG_PIN = 7;

// //Relay 3
// const int DIR_PIN  = 21;
// const int TRIG_PIN = 12;

// Relay 4
// const int DIR_PIN  = 5;
// const int TRIG_PIN = A0;

// ======================================

void setup() {
  pinMode(DIR_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);

  Serial.begin(9600);

  digitalWrite(TRIG_PIN, HIGH);
  delay(20);
}

void loop() {
  
  // Switch one way
  Serial.println("MOVING TO HIGH");
  digitalWrite(DIR_PIN, HIGH);
  delay(10000);
  Serial.println("TRIGGERING LOW");
  digitalWrite(TRIG_PIN, LOW);
  delay(20);
  digitalWrite(TRIG_PIN, HIGH);
  // Switch back

  Serial.println("MOVING TO LOW");
  digitalWrite(DIR_PIN, LOW);
  delay(10000);
  Serial.println("TRIGGERING LOW");
  digitalWrite(TRIG_PIN, LOW);
  delay(20);
  digitalWrite(TRIG_PIN, HIGH);
  delay(20);
}