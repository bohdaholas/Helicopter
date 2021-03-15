#include <Arduino.h>
#include <SPI.h>  // Подключаем библиотеку для работы с SPI-интерфейсом
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01
#define PIN_CE  2  // Номер пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 3 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля

RF24 radio(PIN_CE, PIN_CSN); // Создаём объект radio с указанием выводов CE и CSN

int delay_up_down, delay_rotate;

bool is_close(int VRx, int expected_VRx, int VRy, int expected_VRy) {
  int accuracy = 350;
    if ( abs(VRx - expected_VRx) < accuracy &&
          abs(VRy - expected_VRy) < accuracy) {
        return true;
    }
    return false;
}

void print_joystick(int VRx, int VRy) {
  Serial.print(VRx);
  Serial.print("   ");
  Serial.println(VRy);
}

void read_joysticks() {
  int rotate_VRx = analogRead(A4);
  int rotate_VRy = analogRead(A5);

  int up_down_VRx = analogRead(A0);
  int up_down_VRy = analogRead(A1);

  if (is_close(up_down_VRx, 0, up_down_VRy, 500)) {
    // Maximum speed upwards 
    Serial.println("Up");
    delay_up_down = 0;
  }
  if (is_close(up_down_VRx, 150, up_down_VRy, 500)) {
    // Medium speed upwards 
    Serial.println("Up");
    delay_up_down = 1;
  }

  if (is_close(up_down_VRx, 760, up_down_VRy, 480)) {
    // Medium speed downwards 
    Serial.println("Down");
    delay_up_down = 2;
  }
  if (is_close(up_down_VRx, 1000, up_down_VRy, 480)) {
    // Maximum speed downwards 
    Serial.println("Down");
    delay_up_down = 3;
  }

  if (is_close(rotate_VRx, 1023, rotate_VRy, 1023)) {
    // Maximum left rotation speed 
    Serial.println("Left");
    delay_rotate = 0;
  }
  if (is_close(rotate_VRx, 700, rotate_VRy, 700)) {
    // Medium left rotation speed 
    Serial.println("Left");
    delay_rotate = 1;
  }

  if (is_close(rotate_VRx, 330, rotate_VRy, 330)) {
    // Medium right rotation speed 
    Serial.println("Right");
    delay_rotate = 1;
  }
  if (is_close(rotate_VRx, 0, rotate_VRy, 0)) {
    // Maximum right rotation speed 
    Serial.println("Right");
    delay_rotate = 0;
  }
}

void init_joysticks() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
}

void radio_transmit_init() {
  radio.begin();  // Инициализация модуля NRF24L01
  radio.setChannel(5);// Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_1MBPS);// Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH);// Выбираем высокую мощность передатчика (-6dBm)
  radio.openWritingPipe(0x7878787878LL);// Открываем трубу с уникальным ID
}

void get_and_transmit_data() {
  int delays[2] = {delay_rotate, delay_up_down};
  radio.write(delays, 1);
}

void setup() {
  Serial.begin(9600);
  init_joysticks();
  radio_transmit_init();
}

void loop() {
  read_joysticks();
  get_and_transmit_data();
}
