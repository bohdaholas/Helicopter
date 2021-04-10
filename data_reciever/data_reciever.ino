// nano
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#define PIN_CE  2
#define PIN_CSN 10
RF24 radio(PIN_CE, PIN_CSN);

const int MID_ENGINE_HI = 8;

bool flyable_x = false;
bool flyable_y = false;

int motor_delay_x;
int motor_delay_y;

const int TOP_ENGINE_LEFT = 6;
const int TOP_ENGINE_RIGHT = 7;

const int BACK_ENGINE_LEFT = 4;
const int BACK_ENGINE_RIGHT = 9;

const int COMMON_GROUND = 5;

int input_value[1][2];

void setup() {
  radio.begin();
  radio.setChannel(5);
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openReadingPipe (1, 0x7878787878LL); // Открываем трубу ID передатчика
  radio.startListening();
  Serial.begin(9600);


  for(int i = 4; i<=9; i++){
    pinMode(i, OUTPUT);
  }

  digitalWrite(5, LOW);
}

void vegetate(){

}

bool is_close(int VRx, int expected_VRx) {
  int accuracy = 50;
    if ( abs(VRx - expected_VRx) < accuracy) {
        return true;
    }
    return false;
}



void run_helicopter(){  
  if(is_close(input_value[0][0], 500)){
    flyable_x = false;
  }if(is_close(input_value[0][1], 512)){
    flyable_y = false;
  }
  else{
    motor_delay_x = (1023-input_value[0][0]);
    motor_delay_y = (1023-input_value[0][1]);
    flyable_x = true;
    flyable_y = true;
  }
}

int trigger_helicopter(){
  Serial.print(motor_delay_x);
  Serial.print(" ");
  Serial.println(motor_delay_y);
  if(flyable_x){
    digitalWrite(MID_ENGINE_HI, HIGH);
    digitalWrite(TOP_ENGINE_LEFT, HIGH);
    delay(motor_delay_x);
  }
  if(flyable_y){
    digitalWrite(BACK_ENGINE_LEFT, HIGH);
    delay(motor_delay_y);
  }
  else{
    digitalWrite(MID_ENGINE_HI, LOW);
    digitalWrite(TOP_ENGINE_LEFT, LOW);
    digitalWrite(BACK_ENGINE_LEFT, LOW);
  }
  return 0;
}

void loop() {

  // digitalWrite(MID_ENGINE_HI, HIGH);   // turn the LED on (HIGH is the voltage level)
  // digitalWrite(TOP_ENGINE_LEFT, HIGH);   // turn the LED on (HIGH is the voltage level)
  // digitalWrite(BACK_ENGINE_LEFT, HIGH);   // turn the LED on (HIGH is the voltage level)

  if(radio.available()){ // Если в буфер приёмника поступили данные
    radio.read(&input_value, sizeof(input_value));
    run_helicopter();
    trigger_helicopter();
    // Serial.print(input_value[0][0]);
    // Serial.print(" ");
    // Serial.println(input_value[0][1]);
  }
  // else{
  //   vegetate();
  // }
}
