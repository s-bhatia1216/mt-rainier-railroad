// #include <Servo.h>

// Servo servo9;
// Servo servo10;

// // ===== SETTINGS (EDIT THESE) =====
// int minWait9 = 5000;   // min wait for servo 9 (ms)
// int maxWait9 = 10000;  // max wait for servo 9 (ms)

// int minWait10 = 3000;  // min wait for servo 10 (ms)
// int maxWait10 = 8000;  // max wait for servo 10 (ms)

// int delayOffset10 = 2000; // extra delay before servo 10 starts (ms)
// // =================================

// void setup() {
//   servo9.attach(9);
//   servo10.attach(10);
//   randomSeed(analogRead(0));
// }

// void loop() {

//   // Start both at 0°
//   servo9.write(0);
//   servo10.write(0);

//   // Servo 9 waits
//   delay(random(minWait9, maxWait9));

//   // Servo 10 waits (with extra offset)
//   delay(delayOffset10);
//   delay(random(minWait10, maxWait10));

//   // Move servo 9 to 90°
//   for (int angle = 0; angle <= 90; angle += 3) {
//     servo9.write(angle);
//     delay(5);
//   }

//   // Move servo 10 to 90° (starts later)
//   for (int angle = 0; angle <= 90; angle += 3) {
//     servo10.write(angle);
//     delay(5);
//   }

//   // Wait again (independent-ish feel)
//   delay(random(minWait9, maxWait9));

//   // Return servo 9
//   for (int angle = 90; angle >= 0; angle -= 3) {
//     servo9.write(angle);
//     delay(5);
//   }

//   // Return servo 10 (still offset)
//   delay(delayOffset10);
//   for (int angle = 90; angle >= 0; angle -= 3) {
//     servo10.write(angle);
//     delay(5);
//   }
// }


#include <Servo.h>

Servo s9;
Servo s10;

// ===== SETTINGS =====
int minWait9 = 5000;
int maxWait9 = 10000;

int minWait10 = 3000;
int maxWait10 = 8000;

int offset10 = 2000;   // servo 10 starts later
int speedDelay = 15;   // movement speed
// ====================

// ---- Servo 9 state ----
int angle9 = 0;
int target9 = 0;
unsigned long nextMove9 = 0;

// ---- Servo 10 state ----
int angle10 = 0;
int target10 = 0;
unsigned long nextMove10 = 0;

void setup() {
  s9.attach(9);
  s10.attach(10);

  randomSeed(analogRead(0));

  unsigned long now = millis();

  nextMove9 = now + random(minWait9, maxWait9);
  nextMove10 = now + offset10 + random(minWait10, maxWait10);
}

void loop() {
  unsigned long now = millis();

  // ================= SERVO 9 =================

  // check if it's time to pick a new target
  if (now > nextMove9) {

    if (target9 == 0) {
      target9 = 90;
    } else {
      target9 = 0;
    }

    nextMove9 = now + random(minWait9, maxWait9);
  }

  // move one step toward target
  if (angle9 < target9) {
    angle9 = angle9 + 1;
  }

  if (angle9 > target9) {
    angle9 = angle9 - 1;
  }

  s9.write(angle9);

  // ================= SERVO 10 =================

  // check if it's time to pick a new target
  if (now > nextMove10) {

    if (target10 == 0) {
      target10 = 90;
    } else {
      target10 = 0;
    }

    nextMove10 = now + random(minWait10, maxWait10);
  }

  // move one step toward target
  if (angle10 < target10) {
    angle10 = angle10 + 1;
  }

  if (angle10 > target10) {
    angle10 = angle10 - 1;
  }

  s10.write(angle10);

  // controls how fast everything updates
  delay(speedDelay);
}
