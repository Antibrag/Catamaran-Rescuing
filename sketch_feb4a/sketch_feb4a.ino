// Код катамарана Брагин.А 06.02.2024f

#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.hpp>
#include <RF24.h> 

const byte RIGHT_MOTOR_PIN = 16, LEFT_MOTOR_PIN = 8; //Объявляем пины мотора
const byte FIRST_LED_PIN = 15, SECOND_LED_PIN = 16, THIRD_LED_PIN = 17; //Добавляем пины светодиодов
const byte TRIG_FIRST_SONAR_PIN = 2, TRIG_SECOND_SONAR_PIN = 4, TRIG_THIRD_SONAR_PIN = 6; //Пины trig каждого дальномера
const byte ECHO_FIRST_SONAR_PIN = 3, ECHO_SECOND_SONAR_PIN = 5, ECHO_THIRD_SONAR_PIN = 7; //Пины echo каждого дальномера
const byte NRF_CSN_PIN = 9, NRF_CE_PIN = 10; //Объявляем пины CE и CSN радиомодуля

Servo right_motor, left_motor; //Создаём экземпляры класса Servo
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN); //Создаём объект класса радиомодуля

int nrf_data[2]; //Создаём массив данных для принятия: 0 - значения по оси Y, 1 - значения по оси X

void setup()
{
  Serial.begin(9600);

  //Даём пины моторов нашим объектам
  right_motor.attach(RIGHT_MOTOR_PIN);
  left_motor.attach(LEFT_MOTOR_PIN);

  //Инициализируем Trig порты на выход
  for (byte i = TRIG_FIRST_SONAR_PIN; i <= TRIG_THIRD_SONAR_PIN; i+=2)
    pinMode(i, OUTPUT);

  //Инициализируем Echo порты на вход
  for (byte i = ECHO_FIRST_SONAR_PIN; i <= ECHO_THIRD_SONAR_PIN; i+=2)
    pinMode(i, INPUT);

  // Инициализируем пины светодиодов на выход
  for (int i = FIRST_LED_PIN; i <= THIRD_LED_PIN; i++)
    pinMode(i, OUTPUT);

  radio.begin();  // Инициализация модуля NRF24L01
  radio.setChannel(1); //Выбираем 1 канал связи (2.401 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openReadingPipe (1, 0x7878787878LL); // Открываем трубу ID передатчика
  radio.startListening(); // Начинаем прослушивать открываемую трубу

  digitalWrite(FIRST_LED_PIN, HIGH);

  delay(1000);
  left_motor.writeMicroseconds(2300);
  delay(500);
  left_motor.writeMicroseconds(800);
  delay(800);
}

//Функции блоков -----------------------------------------------------------------

//Создаём функцию передвижения для одного мотора
void move_motor(Servo &motor, int speed)
{
  motor.writeMicroseconds(speed);
  delay(20);
}

//Создаём функцию для передвижения обоих моторов
void move_motors(Servo &right_motor, Servo &left_motor, int speed)
{
  right_motor.writeMicroseconds(speed);
  left_motor.writeMicroseconds(speed);
  delay(20);
}

void get_nrf_data(RF24 &radio)
{
  if(radio.available())
  {
    digitalWrite(SECOND_LED_PIN, HIGH);
    radio.read(&nrf_data, sizeof(nrf_data));
  }
  else
    digitalWrite(SECOND_LED_PIN, LOW);
}

//Создаём функцию получающая информацию о расстоянии с дальномера
int get_sonar_output(byte trig_pin, byte echo_pin)
{
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);

  digitalWrite(trig_pin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trig_pin, LOW);

  //Serial.println(pulseIn(echo_pin, HIGH) / 58);

  return pulseIn(echo_pin, HIGH) / 58;
}

//Создаём функцию проверяющая расстояние с дальномеров
//Если расстояние будет меньше 100 см, то функция вернет true
bool sonars_seen_object(byte duration_cm)
{
  for (byte i = TRIG_FIRST_SONAR_PIN; i <= TRIG_THIRD_SONAR_PIN; i+=2)
  {
    if (get_sonar_output(i, i+1) < duration_cm) 
      return true;
  }
  return false;
}

//Конец блоков функций ------------------------------------------------------------

void loop()
{
  if (sonars_seen_object(50))
    digitalWrite(THIRD_LED_PIN, HIGH);
  else
    digitalWrite(THIRD_LED_PIN, LOW);

  delay(500); 

  move_motor(left_motor, 1200);

  get_nrf_data(radio);
/*
  if (nrf_data[1] > 462 && nrf_data[1] < 562)
    move_motors(right_motor, left_motor, map(nrf_data[0], 0, 1024, 0, 255));
  else if (nrf_data[1] >= 562)
  {
    int left_motor_speed = 255 - map(nrf_data[0], 0, 1024, 0, 255);
    move_motor(right_motor, 255 - left_motor_speed);
    move_motor(left_motor, left_motor_speed);
  }
  else
  {
    int right_motor_speed = 255 - map(nrf_data[0], 0, 1024, 0, 255);
    move_motor(right_motor, right_motor_speed);
    move_motor(left_motor, 255 - right_motor_speed);
  }*/ 
}