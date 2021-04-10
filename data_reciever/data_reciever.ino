// nano
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

int run_helicopter(){
  if(input_value[0][0] == 0 and input_value[0][0] == 1){
    for(int i=0; i<=9; i++){
      digitalWrite(i, LOW);
    }
    return 0;
  }

  for(int i=0; i<=9; i++){
    digitalWrite(i, LOW);
  }
}

void loop() {

  // digitalWrite(MID_ENGINE_HI, HIGH);   // turn the LED on (HIGH is the voltage level)
  // digitalWrite(TOP_ENGINE_LEFT, HIGH);   // turn the LED on (HIGH is the voltage level)
  // digitalWrite(BACK_ENGINE_LEFT, HIGH);   // turn the LED on (HIGH is the voltage level)

  if(radio.available()){ // Если в буфер приёмника поступили данные
    run_helicopter();
    radio.read(&input_value, sizeof(input_value));
    Serial.print(input_value[0][0]);
    Serial.print(" ");
    Serial.println(input_value[0][1]);
  }
  // else{
  //   vegetate();
  // }
}
