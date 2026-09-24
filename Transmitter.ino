#include <TinyGPS++.h>

#define RXD2 2  // LoRa RX pin
#define TXD2 3  // LoRa TX pin
#define sentLED 5

#define RXD_GPS 8 // GPS RX pin (connect to TX of GPS module)
#define TXD_GPS 9 // GPS TX pin (connect to RX of GPS module)

// Hardware serial for LoRa
HardwareSerial SerialLoRa(2); // Use UART2 for LoRa
// Hardware serial for GPS
HardwareSerial SerialGPS(1);  // Use UART1 for GPS

TinyGPSPlus gps; // Create TinyGPS++ object

void setup() {
    pinMode(sentLED, OUTPUT);

    Serial.begin(115200);      // Serial Monitor (for debugging)
    SerialLoRa.begin(115200, SERIAL_8N1, RXD2, TXD2); // LoRa communication
    SerialGPS.begin(9600, SERIAL_8N1, RXD_GPS, TXD_GPS); // GPS communication

    Serial.println("Setting up LoRa module...");
    delay(10);

    // Configure LoRa module
    SerialLoRa.print("AT+RESET\r\n");
    delay(250);
    SerialLoRa.print("AT+IPR=115200\r\n");
    delay(250);
    SerialLoRa.print("AT+NETWORKID=5\r\n");
    delay(250);
    SerialLoRa.print("AT+ADDRESS=1\r\n");
    delay(250);
    SerialLoRa.print("AT+BAND=915000000\r\n");
    delay(250);

    Serial.println("Setup complete. Waiting for GPS data...");
}

void loop() {
    // Process incoming GPS data
    while (SerialGPS.available() > 0) {
        char incomingChar = SerialGPS.read();
        gps.encode(incomingChar); // Parse each incoming character
    }

    // Check if a valid location is available
    if (gps.location.isUpdated()) {
        // Extract GPS data
        double decimalLat = gps.location.lat(); // Latitude in decimal degrees
        double decimalLon = gps.location.lng(); // Longitude in decimal degrees
        int numSatellites = gps.satellites.value(); // Number of satellites

        // Debug: Print the data being sent
        Serial.println("=== Transmitting Data ===");
        Serial.println("Latitude: " + String(decimalLat, 6));
        Serial.println("Longitude: " + String(decimalLon, 6));
        Serial.println("Satellites: " + String(numSatellites));
        Serial.println("=========================");

        // Format data as "lat,lon,satellites"
        String gpsData = String(decimalLat, 6) + "," + String(decimalLon, 6) + "," + String(numSatellites);

        // Send GPS data via LoRa
        SerialLoRa.print("AT+SEND=2,");
        SerialLoRa.print(gpsData.length());
        SerialLoRa.print(",");
        SerialLoRa.print(gpsData);
        SerialLoRa.print("\r\n");

        // Indicate data sent with LED blink
        digitalWrite(sentLED, HIGH);
        delay(250);
        digitalWrite(sentLED, LOW);

        delay(4750); // Short delay between transmissions
    } else {
        // Indicate no new GPS data
        Serial.println("No GPS data available...");
        delay(500);
    }
}
