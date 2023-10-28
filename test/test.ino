#include "ICM20600.h"
#include <Wire.h>

ICM20600 icm20600(true);

const int FLEX_SENSOR_PIN = A0;
const float ALPHA = 0.98;
const float GYRO_CONVERSION_FACTOR = 131.0; // For ±250°/s scale
const int DELAY_TIME = 1000;

double pitch, gyroPitch;
double baselinePitch, baselineFlex;
double pitchThreshold = 15.0; // Adjusted for more specific detection
int flexThreshold = 50;

bool isCalibrating = true;
unsigned long lastUpdateTime = 0;
unsigned long previousMillis = 0;


double calculatePitch(int16_t acc_x, int16_t acc_y, int16_t acc_z) {
    return atan2(-(float)acc_x, sqrt((float)acc_y * acc_y + (float)acc_z * acc_z)) * 180.0 / M_PI;
}

void setup() {
    Wire.begin();
    icm20600.initialize();
    Serial.begin(9600);
  
    Serial.println("Starting Calibration... Maintain a neutral posture!");
    delay(5000);
    calibrateSensors();
    Serial.println("Calibration Complete!");
    isCalibrating = false;
    previousMillis = millis();
}

void calibrateSensors() {
    double totalPitch = 0, totalFlex = 0;
    const int numReadings = 100;
    for (int i = 0; i < numReadings; i++) {
       
        totalPitch += calculatePitch(icm20600.getAccelerationX(), icm20600.getAccelerationY(), icm20600.getAccelerationZ());
        totalFlex += analogRead(FLEX_SENSOR_PIN);
        delay(10); // Small delay between readings
    }
    baselinePitch = totalPitch / numReadings;
    baselineFlex = totalFlex / numReadings;
}


void loop() {
    unsigned long currentMillis = millis();
    double deltaTime = (currentMillis - previousMillis) / 1000.0;
    previousMillis = currentMillis;

    // Update gyro-based pitch
    gyroPitch += icm20600.getGyroscopeY() / GYRO_CONVERSION_FACTOR * deltaTime;

    // Existing accelerometer-based pitch calculation
    double accPitch = calculatePitch(icm20600.getAccelerationX(), icm20600.getAccelerationY(), icm20600.getAccelerationZ());

    // Combine accelerometer and gyro data using a complementary filter
    pitch = ALPHA * (pitch + gyroPitch) + (1.0 - ALPHA) * accPitch;


    detectBadPosture();
    logData();
   
    delay(DELAY_TIME); // You can still use the delay, or replace it with non-blocking code.
}

void detectBadPosture() {
    if (!isCalibrating) {
        bool isBadPosture = (pitch - baselinePitch) > pitchThreshold || 
                            abs(analogRead(FLEX_SENSOR_PIN) - baselineFlex) > flexThreshold;

        if (isBadPosture) {
            Serial.println("Bad Shoulder Posture Detected!");
            Serial.print("Pitch Deviation: "); Serial.println(pitch - baselinePitch);
        }
    }
}

void logData() {
    Serial.print("Pitch: "); Serial.print(pitch);
    Serial.print("\tBaselinePitch Value: "); Serial.print(baselinePitch);
    Serial.print("\tPitch Threshold: "); Serial.print(pitchThreshold);
    Serial.print("\tPitch Deviation: "); Serial.println(pitch - baselinePitch);
    
}

