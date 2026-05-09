// #include <Servo.h>

// Servo innerServo;
// Servo outerServo;

// // states (match initial positions)
// int innerState = 1;  // 1 = 90°, 0 = 0°
// int outerState = 1;

// // timing
// unsigned long innerTimer = 0;
// unsigned long outerTimer = 0;

// // intervals
// unsigned long innerInterval = 5000;
// unsigned long outerInterval = 5000;

// // startup control
// bool started = false;
// unsigned long startTime = 0;

// // ---------------- FIXED BOOLEAN MEANING ----------------
// // both start LOWERED according to your definition
// bool inner_tree_lowered = true;  
// bool outer_tree_lowered = true;

// void setup() {
//   innerServo.attach(9);
//   outerServo.attach(10);

//   // initial phase-shifted positions
//   innerServo.write(90);
//   outerServo.write(0);

//   startTime = millis();
// }

// void loop() {

//   // ---- initial hold phase (5 sec) ----
//   bool hold_phase = (!started && (millis() - startTime < 5000));

//   if (!started) {
//     if (hold_phase) {
//       // still holding, no motion
//       Serial.println("hold_phase=1");
//       return;
//     }

//     innerTimer = millis();
//     outerTimer = millis();

//     innerInterval = random(5000, 10000);
//     outerInterval = random(5000, 10000);

//     started = true;
//   }

//   // ---- INNER SERVO ----
//   if (millis() - innerTimer >= innerInterval) {
//     innerTimer = millis();

//     innerState = !innerState;
//     innerServo.write(innerState ? 90 : 0);

//     inner_tree_lowered = innerState;  // 90 = lowered in your system

//     innerInterval = random(1000, 5000);
//   }

//   // ---- OUTER SERVO ----
//   if (millis() - outerTimer >= outerInterval) {
//     outerTimer = millis();

//     outerState = !outerState;
//     outerServo.write(outerState ? 90 : 0);

//     outer_tree_lowered = outerState;  // 0 = lowered in your system

//     outerInterval = random(5000, 10000);
//   }

//   // ---------------- DEBUG ----------------
//   Serial.print("hold_phase=");
//   Serial.print(hold_phase);

//   Serial.print(" | inner_tree_lowered=");
//   Serial.print(inner_tree_lowered);

//   Serial.print(" | outer_tree_lowered=");
//   Serial.println(outer_tree_lowered);
// }

#include <Servo.h>

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

// states
bool inner_tree_lowered = true;
bool outer_tree_lowered = true;

void setup() {
  Serial.begin(9600);   // IMPORTANT (you were missing this)

  innerServo.attach(9);
  outerServo.attach(10);

  innerServo.write(90);
  outerServo.write(0);

  startTime = millis();
}

void loop() {

  bool hold_phase = (!started && (millis() - startTime < 5000));

  // ---------------- HOLD PHASE ----------------
  if (!started && hold_phase) {

    // still show live status
    Serial.print("hold_phase=1");
    Serial.print(" | inner_tree_lowered=");
    Serial.print(inner_tree_lowered);
    Serial.print(" | outer_tree_lowered=");
    Serial.println(outer_tree_lowered);

    return;
  }

  if (!started) {
    innerTimer = millis();
    outerTimer = millis();

    innerInterval = random(5000, 10000);
    outerInterval = random(5000, 10000);

    started = true;
  }

  // ---------------- INNER SERVO ----------------
  if (millis() - innerTimer >= innerInterval) {
    innerTimer = millis();

    innerState = !innerState;
    innerServo.write(innerState ? 90 : 0);

    inner_tree_lowered = innerState;

    innerInterval = random(1000, 5000);
  }

  // ---------------- OUTER SERVO ----------------
  if (millis() - outerTimer >= outerInterval) {
    outerTimer = millis();

    outerState = !outerState;
    outerServo.write(outerState ? 0 : 90);
    outer_tree_lowered = outerState;

    outerInterval = random(5000, 10000);
  }

  // ---------------- REAL-TIME DEBUG (ALWAYS RUNS) ----------------
  Serial.print("hold_phase=");
  Serial.print(hold_phase);

  Serial.print(" | inner_tree_lowered=");
  Serial.print(inner_tree_lowered);

  Serial.print(" | outer_tree_lowered=");
  Serial.println(outer_tree_lowered);
}