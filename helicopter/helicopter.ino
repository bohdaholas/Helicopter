#include <Arduino.h>
#include <SPI.h>  
#include <nRF24L01.h> 
#include <RF24.h> 
#define PIN_CE  2  
#define PIN_CSN 10 
RF24 radio(PIN_CE, PIN_CSN); 

const int MID_ENGINE_HI = 8; // by default spinning Left
const int TOP_ENGINE_LEFT = 6;
const int TOP_ENGINE_RIGHT = 7;
const int BACK_ENGINE_LEFT = 4;
const int BACK_ENGINE_RIGHT = 9;
const int COMMON_GROUND = 5;

byte gamepad_data[4];

int yaw_delay = 128; // left and right rotation
int lift_delay = 128; // up and down
int roll_delay = 128; // left and right movement

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
  digitalWrite(motor_pin, 1);
}

void turn_off_motor(int motor_pin){
  digitalWrite(motor_pin, 0);
}


void init_radio() {
  radio.begin();
  radio.setChannel(5);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_HIGH); 
  radio.openReadingPipe (1, 0x7878787878LL);
  radio.startListening();
}

void print_gamepad_data() {
  for(int i = 0; i < 3; i++) {
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
  }
}

void setup() {
  Serial.begin(9600);
  init_radio();
  init_helicopter();  
}
 
void loop() {
  while(Serial.available()){  //is there anything to read?
    char getData = Serial.read();  //if yes, read it
  }   // don't do anything with it.
  get_gamepad_data();
  delay(15.625);
  //print_gamepad_data();
  
  Serial.println(yaw_delay);
  if (!is_close(yaw_delay, 128)){
    spin_motor(MID_ENGINE_HI);
    spin_motor(TOP_ENGINE_RIGHT);
  }
  else {
    turn_off_motor(MID_ENGINE_HI);
    turn_off_motor(TOP_ENGINE_RIGHT);
  }
  
  // fly();
}
