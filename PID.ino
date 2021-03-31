#include "GyverPID.h"

// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 13

//пины управления нагревателями
#define heaterPin1 10
#define heaterPin2 11
#define heaterPin3 12

//пины управления светодиодами
#define redLed1 17//красные
#define redLed2 18
#define redLed3 19
#define greenLed1 14//зеленые
#define greenLed2 15
#define greenLed3 16
#define greenLedCommonVcc 2//общей готовности
#define greenLedCommonGnd 4
#define buzzerVcc 8//пищалка
#define buzzerGnd 6

GyverPID regulator1(0.1, 0.05, 0.01, 500);  // коэф. П, коэф. И, коэф. Д, период дискретизации dt (мс)
GyverPID regulator2(0.1, 0.05, 0.01, 500);  // коэф. П, коэф. И, коэф. Д, период дискретизации dt (мс)
GyverPID regulator3(0.1, 0.05, 0.01, 500);  // коэф. П, коэф. И, коэф. Д, период дискретизации dt (мс)

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//текущие температуры датчиков
float currentTemperature1 = 0;
float currentTemperature2 = 0;
float currentTemperature3 = 0;

//коэффициенты регуляторов
float P = 30;
float I = 1.0;
float D = 50;
/*
  //коэффициенты регуляторов
  float P = 50;
  float I = 0.68;
  float D = 30;
*/
//целевая температура
byte targetTemp = 40;

bool readyToWork = 0; //флаг готовности к работе

void setup(void)
{
  //открываем последовательный порт
  Serial.begin(9600);
  Serial.println("Start");

  //назначаем пины светодиодов как выходы
  pinMode(heaterPin1, OUTPUT);
  pinMode(heaterPin2, OUTPUT);
  pinMode(heaterPin3, OUTPUT);

  pinMode(redLed1, OUTPUT);
  pinMode(redLed2, OUTPUT);
  pinMode(redLed3, OUTPUT);

  pinMode(greenLed1, OUTPUT);
  pinMode(greenLed2, OUTPUT);
  pinMode(greenLed3, OUTPUT);

  pinMode(greenLedCommonVcc, OUTPUT);
  pinMode(greenLedCommonGnd, OUTPUT);

  pinMode(buzzerVcc, OUTPUT);
  pinMode(buzzerGnd, OUTPUT);

  regulator1.setDirection(NORMAL); // направление регулирования (NORMAL/REVERSE). ПО УМОЛЧАНИЮ СТОИТ NORMAL
  regulator1.setLimits(0, 255);    // пределы (ставим для 8 битного ШИМ). ПО УМОЛЧАНИЮ СТОЯТ 0 И 255
  regulator1.setpoint = targetTemp;        // сообщаем регулятору температуру, которую он должен поддерживать

  regulator2.setDirection(NORMAL); // направление регулирования (NORMAL/REVERSE). ПО УМОЛЧАНИЮ СТОИТ NORMAL
  regulator2.setLimits(0, 255);    // пределы (ставим для 8 битного ШИМ). ПО УМОЛЧАНИЮ СТОЯТ 0 И 255
  regulator2.setpoint = targetTemp;        // сообщаем регулятору температуру, которую он должен поддерживать

  regulator3.setDirection(NORMAL); // направление регулирования (NORMAL/REVERSE). ПО УМОЛЧАНИЮ СТОИТ NORMAL
  regulator3.setLimits(0, 255);    // пределы (ставим для 8 битного ШИМ). ПО УМОЛЧАНИЮ СТОЯТ 0 И 255
  regulator3.setpoint = targetTemp;        // сообщаем регулятору температуру, которую он должен поддерживать

  //присвоение значений коэффициентов всем регуляторам
  regulator1.Kp = P;
  regulator1.Ki += I;
  regulator1.Kd = D;

  regulator2.Kp = P;
  regulator2.Ki += I;
  regulator2.Kd = D;

  regulator3.Kp = P;
  regulator3.Ki += I;
  regulator3.Kd = D;

  //включаем датчики температуры
  sensors.begin();

  //запрашиваем температуры с датчиков
  sensors.requestTemperatures();

  //записываем текущие температуры с датчиков под различными индексами
  currentTemperature1 = sensors.getTempCByIndex(0);
  currentTemperature2 = sensors.getTempCByIndex(1);
  currentTemperature3 = sensors.getTempCByIndex(2);

  //пока температуры всех датчиков не станут ниже 36 градусов...
  while (currentTemperature1 > 35 or currentTemperature2 > 35 or currentTemperature3 > 35) {

    //включить красный
    digitalWrite(redLed1, 1);
    digitalWrite(redLed2, 1);
    digitalWrite(redLed3, 1);

    //запрашиваем температуры с датчиков
    sensors.requestTemperatures();

    //записываем текущие температуры с датчиков под различными индексами
    currentTemperature1 = sensors.getTempCByIndex(0);
    currentTemperature2 = sensors.getTempCByIndex(1);
    currentTemperature3 = sensors.getTempCByIndex(2);

    //выводим текущие температуры
    Serial.println("Do nothing...cooling (t1,2,3>=36");
    Serial.println(currentTemperature1);
    Serial.println(currentTemperature2);
    Serial.println(currentTemperature3);

    delay(150);

    //выключить красный
    digitalWrite(redLed1, 0);
    digitalWrite(redLed2, 0);
    digitalWrite(redLed3, 0);

    delay(150);
  }
  Serial.println("currentTemperature=================36");

  //пикнуть, чтобы сообщить о начале нагрева
  digitalWrite(buzzerVcc, 1);
  digitalWrite(buzzerGnd, 0);
  delay(250);
  digitalWrite(buzzerVcc, 0);
  digitalWrite(buzzerGnd, 0);
}

void loop()
{
  //запрашиваем температуры с датчиков
  sensors.requestTemperatures();

  //записываем текущие температуры с датчиков под различными индексами
  currentTemperature1 = sensors.getTempCByIndex(0);
  currentTemperature2 = sensors.getTempCByIndex(1);
  currentTemperature3 = sensors.getTempCByIndex(2);

  if (currentTemperature1 == -127) {
    currentTemperature1 = 35;
  }
  if (currentTemperature1 == -127) {
    currentTemperature2 = 35;
  }
  if (currentTemperature1 == -127) {
    currentTemperature3 = 35;
  }

  //выводим текущие температуры
  Serial.println("TEMPERATURES:");
  Serial.println(currentTemperature1);
  Serial.println(currentTemperature2);
  Serial.println(currentTemperature3);

  //если температура 1 датчика меньше 36, то идет нагревание половинной мощностью
  if (currentTemperature1 < 35) {
    digitalWrite(redLed1, 1);
    digitalWrite(greenLed1, 0);

    analogWrite(heaterPin1, 128);  // отправляем на мосфет
    Serial.println("T1<36, heating...");
    Serial.println(currentTemperature1);
  }
  //иначе, если разброс от целевого значения меньше 1 градуса, то включить зеленый светодиод, иначе красный
  else {
    if (abs(currentTemperature1 - targetTemp) <= 1) {
      digitalWrite(redLed1, 0);
      digitalWrite(greenLed1, 1);
    }
    else {
      digitalWrite(redLed1, 1);
      digitalWrite(greenLed1, 0);
    }
    //включаем ШИМ
    regulator1.input = currentTemperature1;   // сообщаем регулятору текущую температуру
    int pwm1 = regulator1.getResultTimer();
    analogWrite(heaterPin1, pwm1);  // отправляем на мосфет
  }



  //если температура 2 датчика меньше 36, то идет нагревание половинной мощностью
  if (currentTemperature2 < 35) {
    digitalWrite(redLed2, 1);
    digitalWrite(greenLed2, 0);

    analogWrite(heaterPin2, 128);  // отправляем на мосфет
    Serial.println("T2<36, heating...");
    Serial.println(currentTemperature2);
  }
  //иначе, если разброс от целевого значения меньше 1 градуса, то включить зеленый светодиод, иначе красный
  else {
    if (abs(currentTemperature2 - targetTemp) <= 1) {
      digitalWrite(redLed2, 0);
      digitalWrite(greenLed2, 1);
    }
    else {
      digitalWrite(redLed2, 1);
      digitalWrite(greenLed2, 0);
    }
    //включаем ШИМ
    regulator2.input = currentTemperature2;   // сообщаем регулятору текущую температуру
    int pwm2 = regulator2.getResultTimer();
    analogWrite(heaterPin2, pwm2);  // отправляем на мосфет
  }



  //если температура 3 датчика меньше 36, то идет нагревание половинной мощностью
  if (currentTemperature3 < 35) {
    digitalWrite(redLed3, 1);
    digitalWrite(greenLed3, 0);

    analogWrite(heaterPin3, 128);  // отправляем на мосфет
    Serial.println("T3<36, heating...");
    Serial.println(currentTemperature3);
  }
  //иначе, если разброс от целевого значения меньше 1 градуса, то включить зеленый светодиод, иначе красный
  else {
    if (abs(currentTemperature3 - targetTemp) <= 1) {
      digitalWrite(redLed3, 0);
      digitalWrite(greenLed3, 1);
    }
    else {
      digitalWrite(redLed3, 1);
      digitalWrite(greenLed3, 0);
    }
    //включаем ШИМ
    regulator3.input = currentTemperature3;   // сообщаем регулятору текущую температуру
    int pwm3 = regulator3.getResultTimer();
    analogWrite(heaterPin3, pwm3);  // отправляем на мосфет
  }



  //если разброс всех температур в норме, то
  if (abs(currentTemperature1 - targetTemp) <= 1 and abs(currentTemperature2 - targetTemp) <= 1 and abs(currentTemperature3 - targetTemp) <= 1) {

    //включить светодиод общей готовности
    digitalWrite(greenLedCommonVcc, 1);
    digitalWrite(greenLedCommonGnd, 0);

    //если не готов к работе, то стать готовым, пикнуть 1 раз
    if (!readyToWork) {
      readyToWork = 1; //готов
      //пикнуть, чтобы сообщить о готовности
      digitalWrite(buzzerVcc, 1);
      digitalWrite(buzzerGnd, 0);
      delay(1000);
      digitalWrite(buzzerVcc, 0);
      digitalWrite(buzzerGnd, 0);
    }
  }

  else {
    //стать неготовым

    if (readyToWork) {
      readyToWork = !readyToWork; // не готов
      digitalWrite(buzzerVcc, 1);
      digitalWrite(buzzerGnd, 0);
      delay(200);
      digitalWrite(buzzerVcc, 0);
      digitalWrite(buzzerGnd, 0);
      delay(200);
      digitalWrite(buzzerVcc, 1);
      digitalWrite(buzzerGnd, 0);
      delay(200);
      digitalWrite(buzzerVcc, 0);
      digitalWrite(buzzerGnd, 0);
      delay(200);
      digitalWrite(buzzerVcc, 1);
      digitalWrite(buzzerGnd, 0);
      delay(200);
      digitalWrite(buzzerVcc, 0);
      digitalWrite(buzzerGnd, 0);
    }

    //выключить светодиод общей готовности
    digitalWrite(greenLedCommonVcc, 0);
    digitalWrite(greenLedCommonGnd, 0);

  }
  delay(750);
}
