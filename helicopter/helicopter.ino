#include <Arduino.h>
#include <SPI.h>  
#include <nRF24L01.h> 
#include <RF24.h> 
#define PIN_CE  2  
#define PIN_CSN 10 
RF24 radio(PIN_CE, PIN_CSN); 


# define MAX_VAL 255
# define BUFFER_SIZE 5

int engine[BUFFER_SIZE];
int counter = 0;


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

//                   0   1   2   3   4   5
byte analogPins[] = {A0, A1, A2, A3, A5, A7};

void findMotors(){
  for(int i = 0; i < 6; i++) {
    pinMode(analogPins[i], OUTPUT);
    analogWrite(analogPins[i], 255);
  }

  for(int i = 0; i < 6; i++){
    analogWrite(analogPins[i], 0);
    delay(1000);
    analogWrite(analogPins[i], 255);
    Serial.println(i);

  }
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
      roll_delay = gamepad_data[1];
      lift_delay = gamepad_data[2];
  }
}

void fly(){
  // Serial.println(yaw_delay);
  if (!is_close(yaw_delay, 128)){
    spin_motor(MID_ENGINE_HI);
    spin_motor(TOP_ENGINE_RIGHT);
  }
  else {
    turn_off_motor(MID_ENGINE_HI);
    turn_off_motor(TOP_ENGINE_RIGHT);
  }

  if (is_close(lift_delay, 0)){
    spin_motor(BACK_ENGINE_LEFT);
  }
  else{
    turn_off_motor(BACK_ENGINE_LEFT);
  }

  if (is_close(lift_delay, 255)){
    spin_motor(BACK_ENGINE_RIGHT);
  }
  else{
    turn_off_motor(BACK_ENGINE_RIGHT);
  }
}


// Delays ////////////////////
float computeBufferAverage(int * buff, int buffSize){
    int buffSum = 0;
    for(int i = 0; i < buffSize; ++i) buffSum += buff[i];

    return (float) buffSum / buffSize;
}

int * intToBinSeq(int num){
    // This functino transforms int number to sequence of binary 0 and 1 according to MAX_VAL (binary vector)
    // e.g. : MAX_VAL = 8, num = 5. intToBinSeq(num) = 10110101

    static int binSeq[MAX_VAL];

    const float frequency = (float) (MAX_VAL - 1) / num;    // frequency of zeros in binary sequence
    float local_frequency = frequency;
    int i = 0;
    for(; i < MAX_VAL; ++i){
        if(i == (int) local_frequency){
            binSeq[i] = 1;
            local_frequency += frequency;
        }
        else{
            binSeq[i] = 0;
        }
    }

    return binSeq;
}


///////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  init_radio();
  // init_helicopter();  
  //pinMode(A0, OUTPUT);
  // pinMode(A3, OUTPUT);

  for(int i = 0; i < BUFFER_SIZE; i++){
    engine[i] = 0;
  }
}
 
void loop() {

//   while(Serial.available()){  //is there anything to read?
//     char getData = Serial.read();  //if yes, read it
//   }   // don't do anything with it.
//   get_gamepad_data();
//   findMotors();
// 
//   delay(15.625);
  // print_gamepad_data();
  
  // analogWrite(A1, 0);
  // analogWrite(A3, 131);


  // A0 - Low; A1 - High -> mid engine right
  // A1 - Low; A0 - High -> mid engine left

  // A2 - Low; A3 - High -> top engine right
  // A3 - Low; A2 - High -> top engine left

  // A0 - Low; A5 - High -> back engine left
  // A5 - Low; A7 - High -> back engine right

    // A0 A5 back left
    // A5 (0) A7(1) back right
    // A2 A3 / A3 A2 - top ok
//   fly();
}
