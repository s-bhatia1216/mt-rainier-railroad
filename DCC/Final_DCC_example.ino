#include <SoftwareSerial.h>
SoftwareSerial SoftSerial(8, 9); // The Rx and Tx pins of ATmega328 -> ACIA (Txd and Rxd respectively)

  byte a = 0x00;
  byte ap = 0x00; // Stores the previous value (0x## is Hexadecimal)
  byte nmask = 0b00001111; // Strips the high nibble (STrain)
  byte smask = 0b11110000; // Strips the low nibble (NTrain)
  byte ntrain = 0;
  byte strain = 0;
  byte ntrainp = 0; // Hold a history of the last value
  byte strainp = 0; 

void setup() {
  Serial.begin(115200); // FTDI cable to computer speed, 115200 might not be reliable
  SoftSerial.begin(9600); // Buad rate for ACIA, which is near it's max 9600
  Serial.println("<1>"); // Turns on track power!
  delay(100);
}

void loop() {
  byte c = 0x31; // Any Byte triggers a reply from SBC with the byte containing STrain and NTrain

  delay(190); // A delay for testing
  SoftSerial.write(c); // Send a byte to the ACIA to trigger it to query the signal sensor board and return STrain|Ntrain in a byte
  SoftSerial.flush(); // Waits for above Tx to finish sending before proceeding
  delay(10); // Waits a short while to allow the next step to process the returned byte, might not be necessary with flush()

  if (SoftSerial.available() > 0) {  // Checks if ACIA sent a byte
    a = SoftSerial.read(); // Get the byte from ACIA
    if (a != ap){  // Checks if the byte received is different from the last one
      ntrain = a & nmask;
      strain = (a & smask) >> 4;
      if (ntrain != ntrainp){
        // Code here runs when NTrain has changed
        if (ntrain != 0){ // Tests for when North block gets occupied (NTrain NOT zero)
          function1();
        }
      }
      if (strain != strainp){
        // Put any code you want when STrain has changed

      }
      ntrainp = ntrain;
      strainp = strain;
      ap = a; //Update the record for next loop
    }
  }
}

 void function1() { // Put your code here e.g. waiting for hall effect sensor then do something like stop the train
    Serial.println("<t 3 50 1>"); // cab speed dir, speed 0..127 or -1 Estop, dir 1=forward 0=reverse  
    delay(5000);
    Serial.println("<0>"); // turns off track power, aka track kill
 }
