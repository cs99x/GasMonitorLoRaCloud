#include <SPI.h>
#include <LoRa.h>

// Pin configuration
#define SS      5   // NSS (CS)
#define RST     14  // RESET
#define DIO0    26  // DIO0

// LoRa parameters
#define BAND  915E6 // Set frequency to 915 MHz (adjust for your region)

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Initializing LoRa module...");

  // Configure LoRa module pins
  LoRa.setPins(SS, RST, DIO0);

  // Initialize LoRa module
  if (!LoRa.begin(BAND)) {
    Serial.println("LoRa initialization failed. Check connections.");
    while (true); // Halt if failed
  }

  Serial.println("LoRa initialized successfully.");
  Serial.println("Waiting for messages...");
}

void loop() {
  // Check if a packet is available
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Read the incoming message
    String incoming = "";
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }

    // Print the received message to the Serial Monitor
    Serial.println("Received message: " + incoming);
  }
}
