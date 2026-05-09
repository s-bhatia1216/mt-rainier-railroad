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

  Serial.println("NEW BOOT");   // <-- add it RIGHT HERE

  Serial.println("BOOT: Serial started");

  while (!Serial) {
    Serial.println("WAIT: Serial not ready...");
    delay(200);
  }

  Serial.println("BOOT: Entered setup()");
  delay(200);

  Serial.println("STEP 1: Starting I2C...");
  Wire.begin();
  Serial.println("STEP 1: I2C started");

  Serial.println("STEP 2: Setting XSHUT pins...");
  pinMode(XSHUT_1, OUTPUT);
  pinMode(XSHUT_2, OUTPUT);
  Serial.println("STEP 2: XSHUT pins set");

  Serial.println("STEP 3: Resetting both sensors (LOW)");
  digitalWrite(XSHUT_1, LOW);
  digitalWrite(XSHUT_2, LOW);
  delay(50);
  Serial.println("STEP 3: Both sensors held in reset");

  // ---------------- SENSOR 1 ----------------
  Serial.println("STEP 4: Powering SENSOR 1");
  digitalWrite(XSHUT_1, HIGH);
  delay(100);
  Serial.println("STEP 4: SENSOR 1 powered");

  Serial.println("STEP 5: Initializing SENSOR 1...");
  if (!sensor1.init()) {
    Serial.println("ERROR: SENSOR 1 init FAILED");
    while (1);
  }
  Serial.println("STEP 5: SENSOR 1 init OK");

  Serial.println("STEP 6: Setting SENSOR 1 address...");
  sensor1.setAddress(ADDR1);
  Serial.println("STEP 6: SENSOR 1 address set to 0x30");

  delay(50);

  // ---------------- SENSOR 2 ----------------
  Serial.println("STEP 7: Powering SENSOR 2");
  digitalWrite(XSHUT_2, HIGH);
  delay(500);
  Serial.println("STEP 7: SENSOR 2 powered");

  Serial.println("STEP 8: Initializing SENSOR 2...");
  if (!sensor2.init()) {
    Serial.println("ERROR: SENSOR 2 init FAILED");
    while (1);
  }
  Serial.println("STEP 8: SENSOR 2 init OK");

  Serial.println("STEP 9: Setting SENSOR 2 address...");
  sensor2.setAddress(ADDR2);
  Serial.println("STEP 9: SENSOR 2 address set to 0x31");

  Serial.println("SETUP COMPLETE: both sensors ready");
}



void loop() {
  Serial.println("LOOP: reading sensors...");

  uint16_t d1 = sensor1.readRangeSingleMillimeters();
  Serial.print("LOOP: sensor1 raw = ");
  Serial.println(d1);

  uint16_t d2 = sensor2.readRangeSingleMillimeters();
  Serial.print("LOOP: sensor2 raw = ");
  Serial.println(d2);

  Serial.print("RESULT -> S1: ");
  Serial.print(d1);
  Serial.print(" mm | S2: ");
  Serial.print(d2);
  Serial.println(" mm");

  Serial.println("LOOP: done cycle\n");
  delay(500);
}