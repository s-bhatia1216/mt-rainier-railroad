
/////////////////////////////
// THIS CODE IS HELPFUL TO SEE WHAT WAS FOUND
////////////////////////////
#include <Wire.h>

#define XSHUT_PIN_1 2
#define XSHUT_PIN_2 3

void setup() {
  Serial.begin(57600);
  delay(3000);

  pinMode(XSHUT_PIN_1, OUTPUT);
  pinMode(XSHUT_PIN_2, OUTPUT);
  digitalWrite(XSHUT_PIN_1, HIGH);
  digitalWrite(XSHUT_PIN_2, HIGH);
  delay(500);

  Wire.begin();
  Serial.println(F("SCANNING..."));

  byte count = 0;
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte err = Wire.endTransmission();
    if (err == 0) {
      Serial.print(F("FOUND: 0x"));
      Serial.println(addr, HEX);
      count++;
    }
    else
    {
      Serial.print(F("NOT FOUND: 0x"));
      Serial.println(addr, HEX);
      count++;
    }
  }
  Serial.print(F("TOTAL: "));
  Serial.println(count);
}

void loop() {}


