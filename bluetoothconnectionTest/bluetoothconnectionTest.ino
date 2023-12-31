#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

const int ledPin = 2;
int counter = 0;

void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32"); // Name of ESP32 Bluetooth device
  
  pinMode(ledPin, OUTPUT);  // Set ledPin as OUTPUT
  digitalWrite(ledPin, LOW);  // Initially set it to LOW (LED OFF)
  
  delay(500);

  Serial.println("Bluetooth device is ready to pair");
}

void loop() {
  // Check if a Bluetooth client is connected
  if (SerialBT.hasClient()) {
    digitalWrite(ledPin, HIGH);  // Turn on the LED
    if(counter == 0){
      Serial.println("Bluetooth device paired");
      SerialBT.print("14\n");  
      delay(10000);
      SerialBT.print("15\n");
      delay(10000);
      SerialBT.print("16\n");
      counter++;
    }
   
  } else {
    digitalWrite(ledPin, LOW);  // Turn off the LED
  }
  
}
