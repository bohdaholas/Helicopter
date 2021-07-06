// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define MAXSIZE 100
#define dt 0.2

typedef struct data {
  float x, y, z;
} Axis3f;

typedef sturct orientation {
  float pitch, yaw, roll;
} state_t;

void imuUpdate(Axis3f acc, Axis3f gyro, state_t *state , float dt);

Adafruit_MPU6050 mpu;
char buf[100];
state_t *state = {.pitch = 0., .yaw = 0., .roll = 0.};
Axis3f acc;
Axis3f gyro;

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
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
}


void loop() {

  /* Get new sensor events with the readings */
  // temp is a pointer to temperature data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  acc.x = a.acceleration.x;
  acc.y = a.acceleration.y;
  acc.z = a.acceleration.z;

  gyro.x = a.gyro.x;
  gyro.y = a.gyro.y;
  gyro.z = a.gyro.z;


  imuUpdate(acc, gyro, state, dt);
  Serial.println("pitch: %f; yaw: %f; roll: %f", state->pitch, state->yaw, state->roll);

  /* Print out the values */
  // Acceleration - m/s^2
  // sprintf(buf, "%f;%f;%f|%f;%f;f", a.acceleration.x, a.acceleration.y, a.acceleration.z,
  //                                 g.gyro.x, g.gyro.y, g.gyro.z);
  // Serial.println(buf);
  // memset(buf, 0, sizeof(buffer));
}

void imuUpdate(Axis3f acc, Axis3f gyro, state_t *state , float dt)
{
  float normalise;
  float ex, ey, ez;
  float halfT = 0.5f * dt;
  float accBuf[3] = {0.f};
  Axis3f tempacc = acc;
 
  gyro.x = gyro.x * DEG2RAD;
  gyro.y = gyro.y * DEG2RAD;
  gyro.z = gyro.z * DEG2RAD;
  
  /* if the three-axis accelerometer is operating, then use Mahony filter*/
  if((acc.x != 0.0f) || (acc.y != 0.0f) || (acc.z != 0.0f))
  {
    normalise = invSqrt(acc.x * acc.x + acc.y * acc.y + acc.z * acc.z);
    acc.x *= normalise;
    acc.y *= normalise;
    acc.z *= normalise;
  
    /*through vector cross product to show measured attitude error of 
               gyrpscope */
    ex = (acc.y * rMat[2][2] - acc.z * rMat[2][1]);
    ey = (acc.z * rMat[2][0] - acc.x * rMat[2][2]);
    ez = (acc.x * rMat[2][1] - acc.y * rMat[2][0]);
  
    /*intergration of error*/
    exInt += Ki * ex * dt ;  
    eyInt += Ki * ey * dt ;
    ezInt += Ki * ez * dt ;
  
    /*PI controller to fuse data*/
    gyro.x += Kp * ex + exInt;
    gyro.y += Kp * ey + eyInt;
    gyro.z += Kp * ez + ezInt;
  }
  
  /*update quaternion through Runge–Kutta methods in discrete system*/
  float q0Last = q0;
  float q1Last = q1;
  float q2Last = q2;
  float q3Last = q3;
  
  q0 += (-q1Last * gyro.x - q2Last * gyro.y - q3Last * gyro.z) * halfT;
  q1 += ( q0Last * gyro.x + q2Last * gyro.z - q3Last * gyro.y) * halfT;
  q2 += ( q0Last * gyro.y - q1Last * gyro.z + q3Last * gyro.x) * halfT;
  q3 += ( q0Last * gyro.z + q1Last * gyro.y - q2Last * gyro.x) * halfT;
  
  normalise = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  
  q0 *= normalise;
  q1 *= normalise;
  q2 *= normalise;
  q3 *= normalise;
  
  /*calculate rotation matrix*/
  imuComputeRotationMatrix(); 
  
  /*calculate euler angle from quaternion*/
  state->pitch = -asinf(rMat[2][0]) * RAD2DEG; 
  state->roll = atan2f(rMat[2][1], rMat[2][2]) * RAD2DEG;
  state->yaw = atan2f(rMat[1][0], rMat[0][0]) * RAD2DEG;
}
