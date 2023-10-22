#include "ICM20600.h"
#include "AK09918.h"
#include <Wire.h>

ICM20600 icm20600(true);
AK09918 ak09918;

const int FLEX_SENSOR_PIN = A0;
const float ALPHA = 0.98;
const float GYRO_CONVERSION_FACTOR = 131.0;
const int DELAY_TIME = 1000;

double roll, pitch;

double baselineRoll, baselinePitch, baselineFlex;
double rollThreshold = 10.0;
double pitchThreshold = 10.0;
int flexThreshold = 50;

bool isCalibrating = true;
unsigned long lastUpdateTime = 0; // For non-blocking delay

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
  

    Serial.println("Starting Calibration... Maintain a neutral posture!");
    delay(5000);
    calibrateSensors();
    Serial.println("Calibration Complete!");
    isCalibrating = false;
}

void calibrateSensors() {
    baselineRoll = calculateRoll(icm20600.getAccelerationX(), icm20600.getAccelerationY(), icm20600.getAccelerationZ());
    baselinePitch = calculatePitch(icm20600.getAccelerationX(), icm20600.getAccelerationY(), icm20600.getAccelerationZ());
    baselineFlex = analogRead(FLEX_SENSOR_PIN);

}

void loop() {
    if (millis() - lastUpdateTime >= DELAY_TIME) {
        lastUpdateTime = millis();

        roll = ALPHA * roll + (1.0 - ALPHA) * calculateRoll(icm20600.getAccelerationX(), icm20600.getAccelerationY(), icm20600.getAccelerationZ());
        pitch = ALPHA * pitch + (1.0 - ALPHA) * calculatePitch(icm20600.getAccelerationX(), icm20600.getAccelerationY(), icm20600.getAccelerationZ());
       

        if (!isCalibrating && (abs(roll - baselineRoll) > rollThreshold || abs(pitch - baselinePitch) > pitchThreshold || abs(analogRead(FLEX_SENSOR_PIN) - baselineFlex) > flexThreshold)) {
            Serial.println("Rounded Shoulders Detected!");
        }

        logData();
    }
}

void logData() {
    Serial.print("Roll: "); Serial.print(roll);
    Serial.print("\tPitch: "); Serial.print(pitch);
    Serial.print("\tFlex Value: "); Serial.println(analogRead(FLEX_SENSOR_PIN));
}
