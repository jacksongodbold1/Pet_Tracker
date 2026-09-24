#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // RX is pin 2, TX is pin 3

void setup() {
  Serial.begin(115200); // For Serial Monitor
  mySerial.begin(115200); // For RYLR896
  Serial.println("Starting...");
  mySerial.print("AT\r\n"); // Send AT command with \r\n
}

void loop() {
  if (mySerial.available()) {
    Serial.write(mySerial.read()); // Display response from RYLR896
  }
  if (Serial.available()) {
    mySerial.write(Serial.read()); // Send input from Serial Monitor to RYLR896
  }
}
