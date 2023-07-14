#include "ICM20600.h"
#include "AK09918.h"
#include <Wire.h>
#include <math.h>

ICM20600 icm20600(true);
AK09918 ak09918;
int16_t gyro_x, gyro_y, gyro_z;
int32_t mag_x, mag_y, mag_z;
double roll, pitch, yaw;

const float alpha = 0.98;  // Complementary filter constant

void setup() {
    Wire.begin();
    icm20600.initialize();
    ak09918.initialize();
    Serial.begin(9600);
    yaw = 0;
}

void loop() {
    // Read gyroscope data
    gyro_x = icm20600.getGyroscopeX();
    gyro_y = icm20600.getGyroscopeY();
    gyro_z = icm20600.getGyroscopeZ();

    // Read magnetometer data
    ak09918.getData(&mag_x, &mag_y, &mag_z);

    // Calculate roll and pitch from accelerometer data
    int16_t acc_x = icm20600.getAccelerationX();
    int16_t acc_y = icm20600.getAccelerationY();
    int16_t acc_z = icm20600.getAccelerationZ();
    roll = atan2((float)acc_y, (float)acc_z) * 180.0 / M_PI;
    pitch = atan2(-(float)acc_x, sqrt((float)acc_y * acc_y + (float)acc_z * acc_z)) * 180.0 / M_PI;

    // Calculate yaw using complementary filter
    yaw += (gyro_z / 131.0) * 0.01;  // Gyroscope integration
    double mag_heading = atan2((double)mag_y, (double)mag_x) * 180.0 / M_PI;
    double gyro_heading = yaw;
    double diff = mag_heading - gyro_heading;

    // Adjust the difference to be within -180 to 180 degrees
    if (diff > 180.0) {
        diff -= 360.0;
    } else if (diff < -180.0) {
        diff += 360.0;
    }

    yaw = alpha * (gyro_heading + diff) + (1.0 - alpha) * mag_heading;

    // Ensure yaw angle is between 0 and 360 degrees
    yaw = fmod(yaw, 360.0);
    if (yaw < 0) {
        yaw += 360.0;
    }

    Serial.println("-----------------------");
    Serial.print("Roll: ");
    Serial.println(roll);
    Serial.print("Pitch: ");
    Serial.println(pitch);
    Serial.print("Yaw: ");
    Serial.println(yaw);
    Serial.println("-----------------------");

    delay(10000);
}