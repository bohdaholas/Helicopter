#include <Arduino.h>
#include <SPI.h>  
#include <nRF24L01.h> 
#include <RF24.h> 
#define PIN_CE  2  
#define PIN_CSN 3
RF24 radio(PIN_CE, PIN_CSN); 

const int BUFFER_SIZE = 4;
byte gamepad_data[BUFFER_SIZE];

void init_radio() {
  radio.begin();  
  radio.setChannel(5); 
  radio.setDataRate (RF24_1MBPS); 
  radio.setPALevel(RF24_PA_HIGH); 
  radio.openWritingPipe(0x7878787878LL); 
}
 
void setup() {
  Serial.begin(115200);
  // Serial.setTimeout(20);
  init_radio();
}

void loop() {
  if (Serial.available() > 0) {
    int data_length = Serial.readBytes(gamepad_data, BUFFER_SIZE);
    radio.write(gamepad_data, sizeof(gamepad_data)); 
    for(int i = 0; i < data_length; i++)
      Serial.print(gamepad_data[i]);
  }
}
