#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor1;
VL53L0X sensor2;

#define XSHUT_PIN_1 2 //inner
#define XSHUT_PIN_2 3 //outer

void setup() {
  Serial.begin(9600); //57600
  delay(3000);

  Serial.println(F("A: START"));

  pinMode(XSHUT_PIN_1, OUTPUT);
  pinMode(XSHUT_PIN_2, OUTPUT);
  digitalWrite(XSHUT_PIN_1, LOW);
  digitalWrite(XSHUT_PIN_2, LOW);
  delay(500);

  Wire.begin();
  Wire.setClock(400000);

  // Bring up sensor 1 alone, assign 0x30
  digitalWrite(XSHUT_PIN_1, HIGH);
  delay(500);
  Serial.println(F("B: SENSOR1 UP"));
  sensor1.setTimeout(500);
  if (!sensor1.init()) {
    Serial.println(F("SENSOR1 FAIL"));
    while (1);
  }
  sensor1.setAddress(0x30);
  Serial.println(F("C: SENSOR1 AT 0x30"));

  // Bring up sensor 2 alone, assign 0x31
  digitalWrite(XSHUT_PIN_2, HIGH);
  delay(500);
  Serial.println(F("D: SENSOR2 UP"));
  sensor2.setTimeout(500);
  if (!sensor2.init()) {
    Serial.println(F("SENSOR2 FAIL"));
    while (1);
  }
  sensor2.setAddress(0x31);
  Serial.println(F("E: SENSOR2 AT 0x31"));

  sensor1.startContinuous();
  sensor2.startContinuous();
  Serial.println(F("F: BOTH RUNNING"));
}

void loop() {
  Serial.print(F("S1: "));
  Serial.print(sensor1.readRangeContinuousMillimeters());
  Serial.print(F("mm  |  S2: "));
  Serial.print(sensor2.readRangeContinuousMillimeters());
  Serial.println(F("mm"));
  delay(300);
}

