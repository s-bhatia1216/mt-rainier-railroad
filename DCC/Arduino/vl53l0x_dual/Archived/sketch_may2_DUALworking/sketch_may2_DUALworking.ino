#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

#define XSHUT_PIN 2  // change to 3 for sensor 2

void setup() {
  Serial.begin(57600);
  delay(3000);

  pinMode(XSHUT_PIN, OUTPUT);
  digitalWrite(XSHUT_PIN, HIGH);
  delay(500);

  Wire.begin();
  sensor.setTimeout(500);

  if (!sensor.init()) {
    Serial.println(F("INIT FAIL"));
    while (1);
  }

  sensor.startContinuous();
  Serial.println(F("READY"));
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