#include <SPI.h>  // Подключаем библиотеку для работы с SPI-интерфейсом
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01
#define PIN_CE  4  // Номер пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 8 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля
RF24 radio(2, 3); // Создаём объект radio с указанием выводов CE и CSN

int input_value[1]; // Создаём массив для приёма значений потенциометра

void setup() {
  radio.begin();  // Инициализация модуля NRF24L01
  radio.setChannel(5); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openReadingPipe (1, 0x7878787878LL); // Открываем трубу ID передатчика
  radio.startListening(); // Начинаем прослушивать открываемую трубу
  Serial.begin(9600);
}

void loop() {
  Serial.print(">:");
  if(radio.available()){ // Если в буфер приёмника поступили данные
    radio.read(&input_value, sizeof(input_value)); // Читаем показания потенциометра
    Serial.println(input_value[0]);
  }
  delay(30);
}
