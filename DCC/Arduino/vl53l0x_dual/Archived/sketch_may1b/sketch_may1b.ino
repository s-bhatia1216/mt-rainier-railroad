#include <Wire.h>
#include <VL53L0X.h>

#define XSHUT_1 2
#define XSHUT_2 3

#define ADDR1 0x30
#define ADDR2 0x31

VL53L0X sensor1;
VL53L0X sensor2;

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("BOOT START");

  // ---------------- I2C INIT ----------------
  Wire.begin();
  Serial.println("I2C initialized");

  // ---------------- FORCE BOTH SENSORS OFF ----------------
  pinMode(XSHUT_1, OUTPUT);
  pinMode(XSHUT_2, OUTPUT);

  digitalWrite(XSHUT_1, LOW);
  digitalWrite(XSHUT_2, LOW);
  delay(200);

  Serial.println("Both sensors held in reset");

  // ---------------- SENSOR 1 INIT ----------------
  Serial.println("Bringing SENSOR 1 up...");
  digitalWrite(XSHUT_1, HIGH);
  delay(300);

  if (!sensor1.init()) {
    Serial.println("SENSOR 1 FAIL");
    while (1);
  }

  Serial.println("SENSOR 1 OK");

  sensor1.setAddress(ADDR1);
  Serial.println("SENSOR 1 address set to 0x30");

  delay(100);

  // ---------------- SENSOR 2 INIT ----------------
  Serial.println("Bringing SENSOR 2 up...");
  digitalWrite(XSHUT_2, HIGH);
  delay(500);

  if (!sensor2.init()) {
    Serial.println("SENSOR 2 FAIL");
    while (1);
  }

  Serial.println("SENSOR 2 OK");

  sensor2.setAddress(ADDR2);
  Serial.println("SENSOR 2 address set to 0x31");

  Serial.println("SETUP COMPLETE");
}

void loop() {
  Serial.println("Reading sensors...");

  uint16_t d1 = sensor1.readRangeSingleMillimeters();
  uint16_t d2 = sensor2.readRangeSingleMillimeters();

  Serial.print("S1: ");
  Serial.print(d1);
  Serial.print(" mm | S2: ");
  Serial.print(d2);
  Serial.println(" mm");

  delay(500);
}