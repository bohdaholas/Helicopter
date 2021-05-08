#include <Arduino.h>
#include <SPI.h>  
#include <nRF24L01.h> 
#include <RF24.h> 
#define PIN_CE  2  
#define PIN_CSN 10 
RF24 radio(PIN_CE, PIN_CSN); 

const int MID_ENGINE_HI = 8;
const int TOP_ENGINE_LEFT = 6;
const int TOP_ENGINE_RIGHT = 7;
const int BACK_ENGINE_LEFT = 4;
const int BACK_ENGINE_RIGHT = 9;
const int COMMON_GROUND = 5;

byte gamepad_data[4];

int yaw_delay; // left and right rotation
int lift_delay; // up and down
int roll_delay; // left and right movement
int pitch_delay; // back and forth

void init_helicopter() {
  for(int i = 4; i <= 9; i++){
    pinMode(i, OUTPUT);
  }
  digitalWrite(COMMON_GROUND, 0);
}

bool is_close(int a, int b) {
  int accuracy = 30;
    if ( abs(a - b) < accuracy) {
        return true;
    }
    return false;
}

void print_motor_delays(int motor_delay_x, int motor_delay_y) {
  Serial.print(motor_delay_x);
  Serial.print(" ");
  Serial.println(motor_delay_y);
}

void reset_all() {
  for(int i = 4; i<=9; i++){
    if (i != 5) {
      pinMode(i, INPUT);
    }
  }
}

void spin_motor(int motor_pin) {
  pinMode(motor_pin, OUTPUT);
  digitalWrite(motor_pin, 1);
}

void hover() {
  Serial.println("hover");
}

void lift() {
  Serial.println("lift");
  spin_motor(TOP_ENGINE_RIGHT);
}

void pitch() {

}

void yaw() {
  Serial.println("yaw");
  spin_motor(BACK_ENGINE_LEFT);
}

void fly(){
  // if (is_close(lift_delay, 128) && is_close(yaw_delay, 128) && 
  //     is_close(pitch_delay, 128) && is_close(roll_delay, 128)) {
  //   hover();
  // }
  if (!(is_close(lift_delay, 128)) && is_close(yaw_delay, 128) && 
      is_close(pitch_delay, 128) && is_close(roll_delay, 128)) {
    lift();
  }
  if (is_close(lift_delay, 128) && !(is_close(yaw_delay, 128)) && 
      is_close(pitch_delay, 128) && is_close(roll_delay, 128)) {
    yaw();
  }
}

void init_radio() {
  radio.begin();
  radio.setChannel(5);
  radio.setDataRate (RF24_1MBPS);
  radio.setPALevel(RF24_PA_HIGH); 
  radio.openReadingPipe (1, 0x7878787878LL);
  radio.startListening();
}

void print_gamepad_data() {
  for(int i = 0; i < 4; i++) {
    Serial.print(gamepad_data[i]);
    Serial.print(" ");
  }
    Serial.println();
}

void get_gamepad_data() {
  if(radio.available()){ 
      radio.read(&gamepad_data, sizeof(gamepad_data)); 
      yaw_delay = gamepad_data[0];
      lift_delay = gamepad_data[1];
      roll_delay = gamepad_data[2];
      pitch_delay = gamepad_data[3];
  }
}

void setup() {
  Serial.begin(9600);
  init_radio();
  init_helicopter();  
}
 
void loop() {
  get_gamepad_data();
  // print_gamepad_data();
  // fly();
}
