#include <M5StickC.h>

// Constants
const int UPDATE_INTERVAL = 1000;   // Update interval in milliseconds

// Variables
float currentAngle = 0.0;          // Current angle
float previousYaw = 0.0;           // Previous yaw value

void setup() {
  M5.begin();
  M5.IMU.Init();
  previousYaw = currentAngle;
}

void loop() {
  M5.update();

  // Calculate the current angle
  currentAngle = currentAngle + calculateYawChange();

  // Normalize the angle between 0 and 360 degrees
  currentAngle = fmod(currentAngle, 360.0);
  if (currentAngle < 0.0) {
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

float getCurrentAngle() {
  // Read the AHRS data
  float pitch, roll, yaw;
  M5.IMU.getAhrsData(&pitch, &roll, &yaw);

  // Calculate the angle using the yaw value
  return yaw * 180.0 / PI;
}

float calculateYawChange() {
  // Read the AHRS data
  float pitch, roll, yaw;
  M5.IMU.getAhrsData(&pitch, &roll, &yaw);

  // Calculate the change in yaw since the previous value
  float yawChange = yaw - previousYaw;

  // Store the current yaw as the previous yaw for the next iteration
  previousYaw = yaw;

  return yawChange * 180.0 / PI;
}
