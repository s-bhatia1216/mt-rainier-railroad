// #include <Servo.h>

// Servo myServo;

// void setup() {
//   myServo.attach(9);  // Arduino pin 9 = ATmega328P PB1 (pin 15)
// }

// void loop() {
//   // Sweep 0 to 180
//   myServo.write(0);
//   delay(20);

//   myServo.write(180);
//   delay(20);
// }

#include <Servo.h>

Servo myServo;

void setup() {
  myServo.attach(11);  // Arduino pin 9 = ATmega328P PB1 (pin 15)
}

void loop() {
  //Sweep 0 to 180
  for (int angle = 0; angle <= 90; angle += 1) {
    myServo.write(angle);
    delay(15);
  }

  // Sweep back 180 to 0
  for (int angle = 90; angle >= 0; angle -= 1) {
    myServo.write(angle);
    delay(15);
  }
  //myServo.write(0);
}