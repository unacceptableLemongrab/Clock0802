#include <Arduino.h>
#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>
#include "Button.h"

static const uint8_t BUTTON_1_PIN = 2;              //пины кнопок
static const uint8_t BUTTON_2_PIN = 15;
static const uint8_t BUTTON_3_PIN = 14;

DateTime now;
uint16_t tempTimeSet[5];          //массив для хранения параметров в режиме настройки

uint8_t setParam = 0;             //номер текущего настраиваемого параметра (0 - часы, 1 - минуты, 2 - день, 3 - месяц, 4 - год)
bool setState = false;            //указывает активность режима настройки

Button button1(BUTTON_1_PIN, 5);  // создание объекта для кнопки 1
Button button2(BUTTON_2_PIN, 5);  // создание объекта для кнопки 2
Button button3(BUTTON_3_PIN, 5);  // создание объекта для кнопки 3
RTC_DS3231 rtc;                   // часы реального времени
LiquidCrystal_I2C lcd(0x27,8,2);  // дисплей на адресе 0x27, 8 символов, 2 строки
byte customChar_p[] = {0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};   //русская буква П
byte customChar_ch[] = { 0x11, 0x11, 0x11, 0x0F, 0x01, 0x01, 0x01, 0x00};  //русская буква Ч
byte customChar_b[] = {0x1F, 0x11, 0x10, 0x1E, 0x11, 0x11, 0x1E, 0x00};   //русская буква Б


void updateCond() {
  button1.filterAvarage();  // вызов метода фильтрации по среднему для кнопки 1
  button2.filterAvarage();  // вызов метода фильтрации по среднему для кнопки 1
  button3.filterAvarage();  // вызов метода фильтрации по среднему для кнопки 1
  
  // реакция на кнопки
  if ( button1.flagClick == true ) {                                // было нажатие кнопки МЕНЬШЕ
    button1.flagClick= false;                                       // сброс признака клика
    if (setState) tempTimeSet[setParam]--;                          //если находимся в режиме настройки, то уменьшаем текущий настраиваемый параметр
  }
  
  if ( button2.flagClick == true && button2.flagPress == false) {   // было нажатие кнопки ОК
    button2.flagClick= false;                                       // сброс признака клика
    if (setState) {
      setParam++;                                                   //при нажатии ОК приступаем к настройке следующего параметра
      lcd.clear();                                                  //очищаем дисплей, чтобы убрать стрелочки "<" и ">", указывающие на текущий настраиваемый параметр
      if (setParam >4) setParam = 0;                                //
    }
  }

  if ( button2.flagClick == true && button2.flagLongPress == true) {// было длительное нажатие кнопки ОК
    button2.flagClick= false;                                       // сброс признака клика
    button2.flagLongPress == false;
    setState = !setState;                                           //заходим или выходим из режима настроек
  }

  if ( button3.flagClick == true ) {                                // было нажатие кнопки
    button3.flagClick= false;                                       // сброс признака клика
    if (setState) tempTimeSet[setParam]++;                          //если находимся в режиме настройки, то увеличиваем текущий настраиваемый параметр
  }
  
}

void lcdPrintTime(const DateTime& dt) {

  lcd.setCursor(0,0);                           //печатаем часы
  if (dt.hour() < 10) lcd.print("0");
  lcd.print(dt.hour());

  lcd.setCursor(3,0);                           //печатаем минуты
  if (dt.minute() < 10) lcd.print("0");
  lcd.print(dt.minute());

  lcd.setCursor(0,1);                           //печатаем день
  if (dt.day() < 10) lcd.print("0");
  lcd.print(dt.day());

  lcd.setCursor(3,1);                           //печатаем месяц
  if (dt.month() < 10) lcd.print("0");
  lcd.print(dt.month());

  lcd.setCursor(6,1);                           //печатаем год
  lcd.print(dt.year()-2000);

  if(setState) {                                //если в режиме установки, то
    lcd.setCursor(5,0);
    lcd.print("set");                           //вместо дня недели печатаем надпись "set"
    switch (setParam) {                         //стрелочкой обозначаем текущий параметр, который настраиваем
      case 0:
        lcd.setCursor(2,0);
        lcd.print("<");
        break;
      case 1:
        lcd.setCursor(2,0);
        lcd.print(">");
        break;
      case 2:
        lcd.setCursor(2,1);
        lcd.print("<");
        break;
      case 3:
        lcd.setCursor(2,1);
        lcd.print(">");
        break;
      case 4:
        lcd.setCursor(5,1);
        lcd.print(">");
        break;
      default:
        //
        break;
    }
  }
  else {                                        //если не находимся в режиме настройки, то
    lcd.setCursor(6,0);
    switch (dt.dayOfTheWeek()) {                //печатаем день недели
      case 0:
        lcd.print("BC");
        break;
      case 1:
        lcd.write(0);
        lcd.print("H");
        break;
      case 2:
        lcd.print("BT");
        break;
      case 3:
        lcd.print("CP");
        break;
      case 4:
        lcd.write(1);
        lcd.print("T");
        break;
      case 5:
        lcd.write(0);
        lcd.print("T");
        break;
      case 6:
        lcd.print("C");
        lcd.write(2);
        break;
      default:
        //ничего
        break;
    }
    lcd.setCursor(2,0);
  if (dt.second()%2 == 0) {                     //печатаем двоеточие между часами и минутами каждую вторую секунду
    lcd.print(":");
  }
  else {
    lcd.print(" ");
  }
    lcd.setCursor(2,1);                         //печатаем слеши между днем/месяцем/годом
    lcd.print("/");
    lcd.setCursor(5,1);
    lcd.print("/");
  }
}

void setClock() {                               //установка часов
  DateTime now2;                                //создаем объект для временного хранения настроек
  lcd.clear();
  setParam = 0;                                 //сбрасываем номер параметра настройки
  now = rtc.now();
  tempTimeSet[0] = now.hour();                  //заполняем массив, хранящий временные параметры настройки данными текущей даты/времени
  tempTimeSet[1] = now.minute();
  tempTimeSet[2] = now.day();
  tempTimeSet[3] = now.month();
  tempTimeSet[4] = now.year()-2000;

  while (setState) {                            //пока не выйдем из режима настройки, делаем:
    updateCond();                               //обновляем состояние кнопок
    now = DateTime(tempTimeSet[4]+2000, tempTimeSet[3], tempTimeSet[2], tempTimeSet[0], tempTimeSet[1], 0);   //заполняем объект настроенными параметрами
    uint32_t unix = now.unixtime();             //преобразуем в юникс тайм
    now2 = DateTime(unix);             //делаем новый объект и устанавливаем в него значение юникс тайм из предыдущего (чтобы не делать ограничения для минут, часов и т.д.)
    lcdPrintTime(now2);                         //выводим на дисплей текущие значения
  }

  rtc.adjust(DateTime(now2));                   //сохраняем в rtc настройки
  lcd.clear();                                  //очищаем дисплей
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, customChar_p);              //создаем кастомные символы (буквы П, Ч, Б)
  lcd.createChar(1, customChar_ch);
  lcd.createChar(2, customChar_b);
  lcd.setCursor(2,1);
  lcd.print("clock");
  delay(700);
  lcd.clear();
}

void loop() {
  now = rtc.now();                              //обновляем объект now из rtc
  lcdPrintTime(now);                            //печатаем на дисплее текущее время
  updateCond();                                 //обновляем состояние кнопок
  if (setState) setClock();                     //если переменная setState = true, заходим в функцию настройки часов
}