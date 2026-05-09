#include <Wire.h>
#include <VL53L0X.h>

#define XSHUT_1 2
#define ADDR1 0x30

VL53L0X sensor1;

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("NEW BOOT - SENSOR 1 ONLY");

  // ---------------- I2C INIT ----------------
  Wire.begin();
  Serial.println("I2C started");

  // ---------------- XSHUT SETUP ----------------
  pinMode(XSHUT_1, OUTPUT);

  // force sensor OFF first
  digitalWrite(XSHUT_1, LOW);
  delay(200);

  Serial.println("Sensor in reset");

  // ---------------- SENSOR 1 INIT ----------------
  Serial.println("Starting SENSOR 1...");
  digitalWrite(XSHUT_1, HIGH);
  delay(300);

  if (!sensor1.init()) {
    Serial.println("FATAL: SENSOR 1 FAILED");
    while (1);
  }

  Serial.println("SENSOR 1 OK");

  sensor1.setAddress(ADDR1);
  Serial.println("SENSOR 1 address set to 0x30");

  Serial.println("SETUP COMPLETE");
}

void loop() {
  uint16_t d1 = sensor1.readRangeSingleMillimeters();

  Serial.print("S1: ");
  Serial.print(d1);
  Serial.println(" mm");

  delay(500);
}