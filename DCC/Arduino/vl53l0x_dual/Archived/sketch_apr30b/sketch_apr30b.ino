#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor1;
VL53L0X sensor2;

#define XSHUT_1 2
#define XSHUT_2 3

#define SENSOR1_ADDR 0x30

void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(XSHUT_1, OUTPUT);
  pinMode(XSHUT_2, OUTPUT);

  // Reset both sensors
  digitalWrite(XSHUT_1, LOW);
  digitalWrite(XSHUT_2, LOW);
  delay(10);

  // --- Sensor 1 ---
  digitalWrite(XSHUT_1, HIGH);
  delay(10);

  sensor1.setTimeout(500);
  if (!sensor1.init()) {
    Serial.println("Failed to detect sensor 1");
    while (1);
  }

  sensor1.setAddress(SENSOR1_ADDR);

  // --- Sensor 2 ---
  digitalWrite(XSHUT_2, HIGH);
  delay(10);

  sensor2.setTimeout(500);
  if (!sensor2.init()) {
    Serial.println("Failed to detect sensor 2");
    while (1);
  }

  // Optional: improve accuracy/speed balance
  sensor1.startContinuous();
  sensor2.startContinuous();

  Serial.println("Sensors initialized.");
}

void loop() {
  uint16_t dist1 = sensor1.readRangeContinuousMillimeters();
  uint16_t dist2 = sensor2.readRangeContinuousMillimeters();

  Serial.print("S1: ");
  if (sensor1.timeoutOccurred()) {
    Serial.print("Timeout");
  } else {
    Serial.print(dist1);
    Serial.print(" mm");
  }

  Serial.print(" | S2: ");
  if (sensor2.timeoutOccurred()) {
    Serial.print("Timeout");
  } else {
    Serial.print(dist2);
    Serial.print(" mm");
  }

  Serial.println();

  delay(50);
}