#include <Servo.h>

// ---------------- SERVOS ----------------
Servo innerServo;
Servo outerServo;

void setup() {
  innerServo.attach(11);  // Arduino pin 11 = ATmega328P pin 17
  outerServo.attach(10);  // Arduino pin 10 = ATmega328P pin 16
}

void loop() {
  //Sweep 0 to 180
  for (int angle = 0; angle <= 90; angle += 1) {
    innerServo.write(angle);
    outerServo.write(90-angle);
    delay(15);
  }

  // Sweep back 180 to 0
  for (int angle = 90; angle >= 0; angle -= 1) {
    innerServo.write(angle);
    outerServo.write(90-angle);
    delay(15);
  }
}