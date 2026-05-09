// #include <Wire.h>
// #include <VL53L0X.h>

// #define XSHUT_1 2
// #define XSHUT_2 3

// #define ADDR1 0x30
// #define ADDR2 0x31

// VL53L0X sensor1;
// VL53L0X sensor2;

// bool sensor2_ok = false;

// void setup() {
//   Serial.begin(9600);  

//   Serial.println("NEW BOOT");   // <-- add it RIGHT HERE

//   Serial.println("BOOT: Serial started");

//   while (!Serial) {
//     Serial.println("WAIT: Serial not ready...");
//     delay(200);
//   }

//   Serial.println("BOOT: Entered setup()");
//   delay(200);

//   Serial.println("STEP 1: Starting I2C...");
//   Wire.begin();
//   Serial.println("STEP 1: I2C started");

//   Serial.println("STEP 2: Setting XSHUT pins...");
//   pinMode(XSHUT_1, OUTPUT);
//   pinMode(XSHUT_2, OUTPUT);
//   Serial.println("STEP 2: XSHUT pins set");

//   Serial.println("STEP 3: Resetting both sensors (LOW)");
//   digitalWrite(XSHUT_1, LOW);
//   digitalWrite(XSHUT_2, LOW);
//   delay(50);
//   Serial.println("STEP 3: Both sensors held in reset");

//   // ---------------- SENSOR 1 ----------------
//   Serial.println("STEP 4: Powering SENSOR 1");
//   digitalWrite(XSHUT_1, HIGH);
//   delay(100);
//   Serial.println("STEP 4: SENSOR 1 powered");

//   Serial.println("STEP 5: Initializing SENSOR 1...");
//   if (!sensor1.init()) {
//     Serial.println("ERROR: SENSOR 1 init FAILED");
//     while (1);
//   }
//   Serial.println("STEP 5: SENSOR 1 init OK");

//   Serial.println("STEP 6: Setting SENSOR 1 address...");
//   sensor1.setAddress(ADDR1);
//   Serial.println("STEP 6: SENSOR 1 address set to 0x30");

//   delay(50);

//   // ---------------- SENSOR 2 ----------------
//   Serial.println("STEP 7: Powering SENSOR 2");
//   digitalWrite(XSHUT_2, HIGH);
//   delay(2000);   // (slightly safer than 500ms)

//   Serial.println("STEP 7: SENSOR 2 powered");

  
//   // ---------------- DEBUG: CHECK I2C (0x29) ----------------
//   Serial.println("CHECKING 0x29 BEFORE SENSOR 2 INIT");

//   Wire.beginTransmission(0x29);
//   byte error = Wire.endTransmission();

//   Serial.print("0x29 response: ");
//   Serial.println(error);

//   Serial.println("CHECKING 0x30 BEFORE SENSOR 2 INIT");

//   Wire.beginTransmission(0x30);
//   byte error2 = Wire.endTransmission();

//   Serial.print("0x30 response: ");
//   Serial.println(error2);

//    Serial.println("CHECKING 0x31 BEFORE SENSOR 2 INIT");

//   Wire.beginTransmission(0x31);
//   byte error3 = Wire.endTransmission();

//   Serial.print("0x31 response: ");
//   Serial.println(error3);


//   // ---------------- POWER SENSOR 2 ----------------
//   Serial.println("Starting SENSOR 2...");

//   digitalWrite(XSHUT_2, HIGH);
//   delay(1000);


//   // ---------------- I2C SAFE STATE ----------------
//   Wire.begin();
//   delay(50);


//   // ---------------- INIT SENSOR 2 ----------------
//   if (!sensor2.init()) {
//     Serial.println("WARNING: SENSOR 2 FAILED");
//     sensor2_ok = false;
//   } else {
//     sensor2.setAddress(ADDR2);
//     Serial.println("SENSOR 2 OK");
//     sensor2_ok = true;
//   }
//   Serial.println("SETUP COMPLETE: both sensors ready");
// }



// void loop() {
//   Serial.println("LOOP: reading sensors...");

//   uint16_t d1 = sensor1.readRangeSingleMillimeters();
//   Serial.print("LOOP: sensor1 raw = ");
//   Serial.println(d1);

//   uint16_t d2 = sensor2.readRangeSingleMillimeters();
//   Serial.print("LOOP: sensor2 raw = ");
//   Serial.println(d2);

//   Serial.print("RESULT -> S1: ");
//   Serial.print(d1);
//   Serial.print(" mm | S2: ");
//   Serial.print(d2);
//   Serial.println(" mm");

//   Serial.println("LOOP: done cycle\n");
//   delay(500);
// }

// #include <Wire.h>

// #define XSHUT_2 3

// void setup() {
//   Serial.begin(9600);
//   delay(1000);

//   pinMode(XSHUT_2, OUTPUT);

//   Serial.println("FORCE LOW");
//   digitalWrite(XSHUT_2, LOW);
//   delay(2000);

//   Serial.println("FORCE HIGH");
//   digitalWrite(XSHUT_2, HIGH);
//   delay(2000);

//   Wire.begin();

//   Wire.beginTransmission(0x29);
//   byte error = Wire.endTransmission();

//   Serial.print("0x29 response: ");
//   Serial.println(error);
// }

// void loop() {}

// void setup() {
//   Serial.begin(9600);
//   delay(1000);

//   pinMode(3, OUTPUT);

//   while (1) {
//     digitalWrite(3, HIGH);
//     Serial.println("HIGH");
//     delay(1000);

//     digitalWrite(3, LOW);
//     Serial.println("LOW");
//     delay(1000);
//   }
// }

// void loop() {}

// #include <Wire.h>
// #include <VL53L0X.h>

// #define XSHUT_2 3
// #define ADDR2 0x31

// VL53L0X sensor2;

// void setup() {
//   Serial.begin(9600);
//   delay(1000);

//   Serial.println("SENSOR 2 ONLY TEST");

//   // ---------------- I2C INIT ----------------
//   Wire.begin();
//   Serial.println("I2C started");

//   // ---------------- XSHUT ----------------
//   pinMode(XSHUT_2, OUTPUT);

//   Serial.println("Reset LOW");
//   digitalWrite(XSHUT_2, LOW);
//   delay(500);

//   Serial.println("Bringing SENSOR 2 HIGH");
//   digitalWrite(XSHUT_2, HIGH);
//   delay(1000);  // important for boot

//   // ---------------- INIT ----------------
//   Serial.println("Initializing SENSOR 2...");

//   if (!sensor2.init()) {
//     Serial.println("FAILED: SENSOR 2 NOT DETECTED");
//     while (1);
//   }

//   Serial.println("SENSOR 2 INIT OK");

//   sensor2.setAddress(ADDR2);
//   Serial.println("Address set to 0x31");

//   Serial.println("READY");
// }

// void loop() {
//   uint16_t distance = sensor2.readRangeSingleMillimeters();

//   Serial.print("Distance: ");
//   Serial.print(distance);
//   Serial.println(" mm");

//   delay(500);
// }

#include <Wire.h>
#include <VL53L0X.h>

#define XSHUT_2 4

VL53L0X sensor2;

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("MIN SENSOR 2 TEST");

  // ---------------- I2C ----------------
  Wire.begin();
  Serial.println("I2C started");

  // ---------------- XSHUT ----------------
  pinMode(XSHUT_2, OUTPUT);

  Serial.println("RESET LOW");
  digitalWrite(XSHUT_2, LOW);
  delay(500);

  Serial.println("XSHUT HIGH");
  digitalWrite(XSHUT_2, HIGH);
  delay(1500);   // IMPORTANT: long boot time

  // ---------------- I2C CHECK (0x29) ----------------
  Serial.println("CHECK 0x29");

  Wire.beginTransmission(0x29);
  byte error = Wire.endTransmission();

  Serial.print("0x29 response: ");
  Serial.println(error);

  // ---------------- INIT ----------------
  Serial.println("INIT SENSOR 2...");

  if (!sensor2.init()) {
    Serial.println("FAILED: NO RESPONSE FROM SENSOR 2");
    while (1);
  }

  Serial.println("SENSOR 2 OK");
}

void loop() {
  uint16_t d = sensor2.readRangeSingleMillimeters();

  Serial.print("Distance: ");
  Serial.print(d);
  Serial.println(" mm");

  delay(500);
}