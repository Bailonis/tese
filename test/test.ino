#include "ICM20600.h"
#include "AK09918.h"
#include <Wire.h>
#include <math.h>

ICM20600 icm20600(true);
AK09918 ak09918;

const int FLEX_SENSOR_PIN = A0; // Replace with the appropriate analog pin connected to the flex sensor
const float ALPHA = 0.98;
const float GYRO_CONVERSION_FACTOR = 131.0;
const int DELAY_TIME = 1000; // Reduced delay to 1 second for better data logging

double roll, pitch, yaw;

double baselineRoll, baselinePitch, baselineFlex; // Baseline values
double rollThreshold = 10.0;  // Define your thresholds
double pitchThreshold = 10.0;
int flexThreshold = 50; // Arbitrary value, adjust as needed

bool isCalibrating = true; // Calibration flag

double calculateRoll(int16_t acc_x, int16_t acc_y, int16_t acc_z) {
    return atan2((float)acc_y, (float)acc_z) * 180.0 / M_PI;
}

double calculatePitch(int16_t acc_x, int16_t acc_y, int16_t acc_z) {
    return atan2(-(float)acc_x, sqrt((float)acc_y * acc_y + (float)acc_z * acc_z)) * 180.0 / M_PI;
}


void setup() {
    Wire.begin();
    icm20600.initialize();
    ak09918.initialize();
    Serial.begin(9600);
    yaw = 0;

    Serial.println("Starting Calibration... Maintain a neutral posture!");
    delay(5000); // Give 5 seconds for user to assume a neutral posture
    calibrateSensors(); // Establish baseline
    Serial.println("Calibration Complete!");
    isCalibrating = false;
}

void calibrateSensors() {
    // Fetch and set baseline values
    baselineRoll = calculateRoll(icm20600.getAccelerationX(), icm20600.getAccelerationY(), icm20600.getAccelerationZ());
    baselinePitch = calculatePitch(icm20600.getAccelerationX(), icm20600.getAccelerationY(), icm20600.getAccelerationZ());
    baselineFlex = analogRead(FLEX_SENSOR_PIN);
}

void loop() {
    // ... [Rest of the orientation code] ...

    // Detect rounded shoulders based on deviations from baseline
    if (!isCalibrating && (abs(roll - baselineRoll) > rollThreshold || abs(pitch - baselinePitch) > pitchThreshold || abs(analogRead(FLEX_SENSOR_PIN) - baselineFlex) > flexThreshold)) {
        Serial.println("Rounded Shoulders Detected!");
    }

    // Data logging (assuming SD card module or other storage means)
    logData();

    delay(DELAY_TIME);
}

void logData() {
    // Store data in a preferred manner. Here, we're just printing to the serial monitor
    Serial.print("Roll: "); Serial.print(roll);
    Serial.print("\tPitch: "); Serial.print(pitch);
    Serial.print("\tFlex Value: "); Serial.println(analogRead(FLEX_SENSOR_PIN));
}
