/*
Button.h - библиотека для цифровой обработки сигналов контактов кнопок
 и сигналов других компонентов параллельным процессом
 
В параллельном процессе должен регулярно вызываться один из методов:
    void  scanState();    // метод проверки ожидание стабильного состояния сигнала
    void  filterAvarage(); // метод фильтрации сигнала по среднему значению
 
В результате формируются признаки:
 
 для метода scanState():
 - при нажатой кнопке flagPress= true 
 - при отжатой кнопке flagPress= false
 - при нажатии на кнопку flagClick= true

 для метода filterAvarage() :
 - при сигнале низкого уровня flagPress= true 
 - при сигнале высокого уровня flagPress= false
 - при изменении состояния с высокого на низкий flagClick= true

Объект типа Button при создании имеет параметры:
 - номер вывода, к которому подключена кнопка или сигнал
 - время обработки сигнала (умножается на период вызова метода scanState() или filterAvarage()

 Button button1(12, 15);  // создание объекта для кнопки, подключенной к 12 выводу 
 с временем фильтрации 30 мс (при цикле 2 мс)

 Библиотека разработана Калининым Эдуардом
 http://mypractic.ru/urok-8-cifrovaya-filtraciya-signalov-v-programmax-dlya-arduino.html
 
*/

// проверка, что библиотека еще не подключена
#ifndef Button_h // если библиотека Button не подключена
#define Button_h // тогда подключаем ее

#include "Arduino.h"

// класс обработки сигналов
class Button {
  public:
    Button(byte pin, byte timeButton);  // конструктор
    bool flagPress;    // признак кнопка нажата (сигнал в низком уровне)
    bool flagLongPress;
    bool flagClick;    // признак клика кнопки (фронт)
    void  scanState();    // метод проверки ожидание стабильного состояния сигнала
    void  filterAvarage(); // метод фильтрации по среднему значению
    void setPinTime(byte pin, byte timeButton); // установка номера вывода и времени фильтрации
 
 private:
    uint8_t  _buttonCount;    // счетчик времени фильтрации   
    uint8_t _timeButton;      // время фильтрации
    uint8_t _pin;             // номер вывода
    uint32_t timePress;       // время последнего нажатия
    
};

#endif
