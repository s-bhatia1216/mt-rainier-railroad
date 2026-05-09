#include <Wire.h>
#include <VL53L0X.h>  // Pololu library

VL53L0X sensor;

#define XSHUT_PIN 3

void setup() {
  Serial.begin(57600);
  delay(3000);

  Serial.println(F("A: START"));

  pinMode(XSHUT_PIN, OUTPUT);
  digitalWrite(XSHUT_PIN, LOW);
  delay(50);
  digitalWrite(XSHUT_PIN, HIGH);
  delay(200);

  Serial.println(F("B: XSHUT HIGH"));

  Wire.begin();

  sensor.setTimeout(500);

  if (!sensor.init()) {
    Serial.println(F("C: INIT FAIL"));
    while (1);
  }

  sensor.startContinuous();
  Serial.println(F("D: INIT OK"));
}

void loop() {
  uint16_t dist = sensor.readRangeContinuousMillimeters();

  if (!sensor.timeoutOccurred()) {
    Serial.print(F("DIST mm: "));
    Serial.println(dist);
  } else {
    Serial.println(F("TIMEOUT"));
  }

  delay(300);
}