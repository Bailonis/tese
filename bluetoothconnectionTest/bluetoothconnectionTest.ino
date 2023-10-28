#include "BluetoothSerial.h"
#include <WiFi.h>

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32"); // Name of your ESP32 Bluetooth device
  delay(500);
   // Get the MAC address of the ESP32
  String mac = WiFi.macAddress();
  Serial.println("MAC address of this ESP32: " + mac);
  Serial.println("Bluetooth device is ready to pair");
 
}

void loop() {
  if (SerialBT.available()) {
    char readChar = SerialBT.read();
    SerialBT.write(readChar); // Echo whatever is received
    Serial.write(readChar);   // Also print to serial for debugging
  }
}


