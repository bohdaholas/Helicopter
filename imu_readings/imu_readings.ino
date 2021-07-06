/*
 * To change the output type, change the declaration of DEBUG to true
 * 
 * Filter is taken from the Sebastian O.H. Madgwick's paper:
 * An efficient orientation filter for inertial and inertial/magnetic sensor arrays
 */
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>

// System constants
#define deltat 0.2f // sampling period in seconds (shown as 1 ms)
#define gyroMeasError 3.14159265358979f * (5.0f / 180.0f) // gyroscope measurement error in rad/s (shown as 5 deg/s)
#define beta sqrt(3.0f / 4.0f) * gyroMeasError // compute beta
#define MAXSIZE 100
#define CALIBRATION 30
#define DEG(a) (int)((a * 4068) / 71)
#define DEBUG false

// types
typedef struct state {
   float pitch, yaw, roll;
} Orientation;

// Functions
void filterUpdate(float w_x, float w_y, float w_z, float a_x, float a_y, float a_z);
void quat2euler(Orientation *orient, float SEq_1, float SEq_2, float SEq_3 , float SEq_4);
void printOrientation(Orientation *orient);

// Global variables
Adafruit_MPU6050 mpu;
Orientation orient;
float SEq_1 = 1.0f, SEq_2 = 0.0f, SEq_3 = 0.0f, SEq_4 = 0.0f; // estimated orientation quaternion elements with initial conditions
float gyro_drift_x, gyro_drift_y, gyro_drift_z;
int iteration = 0;
bool calibrated = false;

void setup(void) {
  Serial.begin(115200);

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  // settings
  // Measurements are in m/s2
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  // Measurements are in rad / s
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
 
  // ! Whenewer you change it, make sure that
  // df in the definitions is changed too.
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
}


void loop() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, _;
  mpu.getEvent(&a, &g, &_);

  // counting the mean of gyro's measurement, to determine its drift
  if (iteration < CALIBRATION) {
      gyro_drift_x += g.gyro.x;
      gyro_drift_y += g.gyro.y;
      gyro_drift_z += g.gyro.z;
  }

  if (!calibrated && iteration >= CALIBRATION) {
    gyro_drift_x /= CALIBRATION;
    gyro_drift_y /= CALIBRATION;
    gyro_drift_z /= CALIBRATION;
    calibrated = !calibrated;
  }

  // calibrating the gyro
  if (calibrated) {
    g.gyro.x -= gyro_drift_x;
    g.gyro.y -= gyro_drift_y;
    g.gyro.z -= gyro_drift_z;
  }

  filterUpdate(g.gyro.z, g.gyro.y, g.gyro.z,
               a.acceleration.x, a.acceleration.y, a.acceleration.z);
  
  // converts the resulting quaterinon into euler angles and
  // prints it out (to change the output, change the definition of debug)
  if (iteration > CALIBRATION * 2) {
 
     quat2euler(&orient, SEq_1, SEq_2, SEq_3, SEq_4);
     
     if (DEBUG) {
        const char *format = "Orientation: yaw %d pitch %d roll %d";
        printOrientation(&orient, format);
     } else {
        const char *format = "%d;%d;%d";
        printOrientation(&orient, format);
     }
  }

  // to avoid overflow when using it long time
  // (though it may be useful if one want to get
  // recalibration after some time (it will `reset` the loop))
  if (iteration < CALIBRATION * 3) {
    iteration++;
  }
  delay(200);
}


/* Converts a quaternion that represents current orientation into euler angles.
 * ! Modifies the orientation global structure, therefore it may be convenient to
 * use it afterwards
 */
void quat2euler(Orientation *orient, float SEq_1, float SEq_2, float SEq_3 , float SEq_4) {
  orient->yaw = atan2(2.0f * (SEq_2 * SEq_3 + SEq_1 * SEq_4), SEq_1 * SEq_1 + SEq_2 * SEq_2 - SEq_3 * SEq_3 - SEq_4 * SEq_4);
  orient->pitch = -asin(2.0f * (SEq_2 * SEq_4 - SEq_1 * SEq_3));
  orient->roll = atan2(2.0f * (SEq_1 * SEq_2 + SEq_3 * SEq_4), SEq_1 * SEq_1 - SEq_2 * SEq_2 - SEq_3 * SEq_3 + SEq_4 * SEq_4);

}


/* Prints into serial port the current orientation in format, specified
 * by the arg given to it (a string);
 */
void printOrientation(Orientation *orient, const char *format) {
  char buf[MAXSIZE];
  sprintf(buf, format, DEG(orient->yaw), DEG(orient->pitch), DEG(orient->roll));
  Serial.println(buf);
}


/* IMU filter (combines gyro and acc measurements)
 * 
 * Filter is taken from the Sebastian O.H. Madgwick's paper:
 *      An efficient orientation filter for inertial and inertial/magnetic sensor arrays
 *      
 * Read it to get the proper understanding of its usage
 */
void filterUpdate(float w_x, float w_y, float w_z, float a_x, float a_y, float a_z)
{
    // Local system variables
    float norm; // vector norm
    float SEqDot_omega_1, SEqDot_omega_2, SEqDot_omega_3, SEqDot_omega_4; // quaternion derrivative from gyroscopes elements
    float f_1, f_2, f_3; // objective function elements
    float J_11or24, J_12or23, J_13or22, J_14or21, J_32, J_33; // objective function Jacobian elements
    float SEqHatDot_1, SEqHatDot_2, SEqHatDot_3, SEqHatDot_4; // estimated direction of the gyroscope error
    // Axulirary variables to avoid reapeated calcualtions
    float halfSEq_1 = 0.5f * SEq_1;
    float halfSEq_2 = 0.5f * SEq_2;
    float halfSEq_3 = 0.5f * SEq_3;
    float halfSEq_4 = 0.5f * SEq_4;
    float twoSEq_1 = 2.0f * SEq_1;
    float twoSEq_2 = 2.0f * SEq_2;
    float twoSEq_3 = 2.0f * SEq_3;
    
    
    // Normalise the accelerometer measurement
    norm = sqrt(a_x * a_x + a_y * a_y + a_z * a_z);
    a_x /= norm;
    a_y /= norm;
    a_z /= norm;
    
    // Compute the objective function and Jacobian
    f_1 = twoSEq_2 * SEq_4 - twoSEq_1 * SEq_3 - a_x;
    f_2 = twoSEq_1 * SEq_2 + twoSEq_3 * SEq_4 - a_y;
    f_3 = 1.0f - twoSEq_2 * SEq_2 - twoSEq_3 * SEq_3 - a_z;
    J_11or24 = twoSEq_3; // J_11 negated in matrix multiplication
    J_12or23 = 2.0f * SEq_4;
    J_13or22 = twoSEq_1; // J_12 negated in matrix multiplication
    J_14or21 = twoSEq_2;
    J_32 = 2.0f * J_14or21; // negated in matrix multiplication
    J_33 = 2.0f * J_11or24; // negated in matrix multiplication
    
    // Compute the gradient (matrix multiplication)
    SEqHatDot_1 = J_14or21 * f_2 - J_11or24 * f_1;
    SEqHatDot_2 = J_12or23 * f_1 + J_13or22 * f_2 - J_32 * f_3;
    SEqHatDot_3 = J_12or23 * f_2 - J_33 * f_3 - J_13or22 * f_1;
    SEqHatDot_4 = J_14or21 * f_1 + J_11or24 * f_2;
    
    // Normalise the gradient
    norm = sqrt(SEqHatDot_1 * SEqHatDot_1 + SEqHatDot_2 * SEqHatDot_2 + SEqHatDot_3 * SEqHatDot_3 + SEqHatDot_4 * SEqHatDot_4);
    SEqHatDot_1 /= norm;
    SEqHatDot_2 /= norm;
    SEqHatDot_3 /= norm;
    SEqHatDot_4 /= norm;
    
    // Compute the quaternion derrivative measured by gyroscopes
    SEqDot_omega_1 = -halfSEq_2 * w_x - halfSEq_3 * w_y - halfSEq_4 * w_z;
    SEqDot_omega_2 = halfSEq_1 * w_x + halfSEq_3 * w_z - halfSEq_4 * w_y;
    SEqDot_omega_3 = halfSEq_1 * w_y - halfSEq_2 * w_z + halfSEq_4 * w_x;
    SEqDot_omega_4 = halfSEq_1 * w_z + halfSEq_2 * w_y - halfSEq_3 * w_x;
    
    // Compute then integrate the estimated quaternion derrivative
    SEq_1 += (SEqDot_omega_1 - (beta * SEqHatDot_1)) * deltat;
    SEq_2 += (SEqDot_omega_2 - (beta * SEqHatDot_2)) * deltat;
    SEq_3 += (SEqDot_omega_3 - (beta * SEqHatDot_3)) * deltat;
    SEq_4 += (SEqDot_omega_4 - (beta * SEqHatDot_4)) * deltat;
    
    // Normalise quaternion
    norm = sqrt(SEq_1 * SEq_1 + SEq_2 * SEq_2 + SEq_3 * SEq_3 + SEq_4 * SEq_4);
    SEq_1 /= norm;
    SEq_2 /= norm;
    SEq_3 /= norm;
    SEq_4 /= norm;
}
