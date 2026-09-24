#include <LiquidCrystal.h>

#define RXD2 2 // ESP32 RX pin
#define TXD2 3 // ESP32 TX pin
#define receivedLED 5

// Initialize the LCD with your pin setup (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(6, 7, 8, 9, 10, 11);

void setup() {
    pinMode(receivedLED, OUTPUT);

    Serial.begin(115200); // Serial Monitor
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); // LoRa communication

    // Initialize the LCD
    lcd.begin(16, 2); // 16x2 LCD dimensions
    lcd.print("Waiting for msg");

    // Configure LoRa module
    Serial.println("Setting up LoRa module...");
    delay(10);
    Serial2.print("AT+RESET\r\n");
    delay(250);
    Serial2.print("AT+IPR=115200\r\n");
    delay(250);
    Serial2.print("AT+NETWORKID=5\r\n");
    delay(250);
    Serial2.print("AT+ADDRESS=2\r\n");
    delay(250);
}

void loop() {
    if (Serial2.available() > 0) {
        String receivedData = ""; // Buffer for the incoming string

        // Read the entire incoming message
        while (Serial2.available() > 0) {
            char incomingChar = Serial2.read();
            receivedData += incomingChar;
            delay(10); // Small delay for complete message reception
        }

        // Display raw data on Serial Monitor
        Serial.println("Raw Data: " + receivedData);

        // Check if the data starts with "+RCV="
        if (receivedData.startsWith("+RCV=")) {
            // Parse the received data
            int firstComma = receivedData.indexOf(','); // First comma
            int secondComma = receivedData.indexOf(',', firstComma + 1); // Second comma
            int thirdComma = receivedData.indexOf(',', secondComma + 1); // Third comma
            int fourthComma = receivedData.indexOf(',', thirdComma + 1); // Fourth comma (for satellites)

            // Extract latitude, longitude, and satellite count
            String latitude = receivedData.substring(secondComma + 1, thirdComma); // Extract latitude
            String longitude = receivedData.substring(thirdComma + 1, fourthComma); // Extract longitude
            String satellites = receivedData.substring(fourthComma + 1); // Extract satellite count

            // Debug: Print extracted values
            Serial.println("Latitude: " + latitude);
            Serial.println("Longitude: " + longitude);
            Serial.println("Satellites: " + satellites);

            // Update LCD with latitude, longitude, and satellite count
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Lat: ");
            lcd.print(latitude); // Display latitude
            lcd.setCursor(0, 1);
            lcd.print("Lon: ");
            lcd.print(longitude); // Display longitude

            // Blink LED to indicate message received
            digitalWrite(receivedLED, HIGH);
            delay(250);
            digitalWrite(receivedLED, LOW);

            // Add satellites information to the Serial Monitor
            Serial.println("Number of Satellites: " + satellites);
        }
    }

    delay(100); // Short delay before the next loop iteration
}
