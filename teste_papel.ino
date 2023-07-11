#include <M5StickC.h>

// Constants
const int UPDATE_INTERVAL = 1000;   // Update interval in milliseconds

// Variables
float initialAngle = 0.0;          // Initial angle
float currentAngle = 0.0;          // Current angle


void setup() {
  M5.begin();
  M5.IMU.Init();
  initialAngle = 0;       // Store the initial angle
}

void loop() {
  M5.update();

  // Read the gyroscope data for X and Y axes
  float gx, gy, gz;
  M5.IMU.getGyroData(&gx, &gy, &gz);

  currentAngle += (gx * UPDATE_INTERVAL / 1000.0);

  // Normalize the angle between 0 and 360 degrees
  if (currentAngle >= 360.0) {
    currentAngle -= 360.0;
  } else if (currentAngle < 0.0) {
    currentAngle += 360.0;
  }

  // Clear the display
  M5.Lcd.fillScreen(BLACK);

  // Display the current angle
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(15, 10);
  M5.Lcd.print("Current");
  M5.Lcd.setCursor(15, 20);
  M5.Lcd.print("Angle");
  M5.Lcd.setCursor(15, 30);
  M5.Lcd.print(currentAngle);
  M5.Lcd.print("º");

  delay(UPDATE_INTERVAL);
}


