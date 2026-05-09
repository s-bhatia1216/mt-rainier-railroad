#include <Wire.h>
#include <VL53L0X.h>

#define XSHUT_1 2
#define XSHUT_2 3

#define ADDR1 0x30
#define ADDR2 0x31

VL53L0X sensor1;
VL53L0X sensor2;

bool sensor2_ok = false;

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("NEW BOOT");

  // ---------------- I2C ----------------
  Wire.begin();
  Serial.println("I2C started");

  // ---------------- XSHUT SETUP ----------------
  pinMode(XSHUT_1, OUTPUT);
  pinMode(XSHUT_2, OUTPUT);

  digitalWrite(XSHUT_1, LOW);
  digitalWrite(XSHUT_2, LOW);
  delay(200);

  Serial.println("Both sensors in reset");

  // ---------------- SENSOR 1 (REQUIRED) ----------------
  Serial.println("Starting SENSOR 1...");
  digitalWrite(XSHUT_1, HIGH);
  delay(300);

  if (!sensor1.init()) {
    Serial.println("FATAL: SENSOR 1 FAILED");
    while (1);
  }

  Serial.println("SENSOR 1 OK");
  sensor1.setAddress(ADDR1);
  Serial.println("SENSOR 1 address set");

  delay(100);

  // ---------------- SENSOR 2 (OPTIONAL) ----------------
  Serial.println("Starting SENSOR 2...");
  digitalWrite(XSHUT_2, HIGH);
  delay(500);

  if (!sensor2.init()) {
    Serial.println("WARNING: SENSOR 2 NOT DETECTED");
    sensor2_ok = false;
  } else {
    sensor2.setAddress(ADDR2);
    Serial.println("SENSOR 2 OK");
    sensor2_ok = true;
  }

  Serial.println("SETUP COMPLETE");
}

void loop() {
  Serial.println("Reading sensor 1...");

  uint16_t d1 = sensor1.readRangeSingleMillimeters();

  Serial.print("S1: ");
  Serial.print(d1);
  Serial.print(" mm");

  // ---------------- OPTIONAL SENSOR 2 ----------------
  if (sensor2_ok) {
    uint16_t d2 = sensor2.readRangeSingleMillimeters();

    Serial.print(" | S2: ");
    Serial.print(d2);
    Serial.print(" mm");
  } else {
    Serial.print(" | S2: OFFLINE");
  }

  Serial.println();
  delay(500);
}