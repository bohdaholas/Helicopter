#include <Arduino.h>

int x_diff, y_diff;

bool is_close(int VRx, int expected_VRx, int VRy, int expected_VRy) {
  int accuracy = 350;
    if ( abs(VRx - expected_VRx) < accuracy &&
          abs(VRy - expected_VRy) < accuracy) {
        return true;
    }
    return false;
}

void read_joystick() {
  int VRx = analogRead(A0);
  int VRy = analogRead(A1);
  if (is_close(VRx, 0, VRy, 500)) {
    Serial.println("Up");
    x_diff = 0;
    y_diff = 1;
  }
  if (is_close(VRx, 980, VRy, 480)) {
    Serial.println("Down");
    x_diff = 0;
    y_diff = -1;
  }
  if (is_close(VRx, 500, VRy, 1000)) {
    Serial.println("Left");
    x_diff = -1;
    y_diff = 0;
  }
  if (is_close(VRx, 500, VRy, 0)) {
    Serial.println("Right");
    x_diff = 1;
    y_diff = 0;
  }
}

void init_joystick() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
}

void setup() {
  Serial.begin(9600);
  init_joystick();
}

void loop() {
  read_joystick();
}






