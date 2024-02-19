#include <SPI.h>
#include <nRF24L01.h> 
#include <RF24.h>  

#define VRX_PIN A1
#define VRY_PIN A2

RF24 radio(NRF_CE, NRF_CSN); //Создаём объект класса радиомодуля

const byte NRF_CSN_PIN = 9, NRF_CE_PIN = 10; //Объявляем пины CE и CSN радиомодуля

int nrf_data[2]; //Создаём массив данных для принятия: 0 - значения по оси Y, 1 - значения по оси X

void setup()
{
  radio.begin();  // Инициализация модуля NRF24L01
  radio.setChannel(1); //Выбираем 1 канал связи (2.401 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openWritingPipe (0x7878787878LL); // Открываем трубу ID
}

void loop()
{
  nrf_data[0] = analogRead(VRX_PIN);
  nrf_data[1] = analogRead(VRY_PIN);

  radio.write(nrf_data, 2);
}
