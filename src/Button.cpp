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

#include "Arduino.h"
#include "Button.h"
// метод фильтрации сигнала по среднему значению
// при сигнале низкого уровня flagPress= true 
// при сигнале высокого уровня flagPress= false
// при изменении состояния с высокого на низкий flagClick= true
void Button::filterAvarage() {

 if ( flagPress != digitalRead(_pin) ) {
     //  состояние кнопки осталось прежним
     if ( _buttonCount != 0 ) _buttonCount--; // счетчик подтверждений - 1 с ограничением на 0 
  }
  else {
     // состояние кнопки изменилось
     _buttonCount++;   // +1 к счетчику подтверждений
     //Serial.print("button count: ");
     //Serial.println(_buttonCount);
     if ( _buttonCount >= _timeButton ) {
        // состояние сигнала достигло порога _timeButton
        flagPress= ! flagPress; // инверсия признака состояния
        _buttonCount= 0;  // сброс счетчика подтверждений
        flagLongPress = false;
        if ( flagPress == true ) {
          flagClick= true; // признак клика кнопки
          //Serial.println("CLICK");
          timePress = millis();      
        }
     }    
  }
  if ( flagPress == true && millis() - timePress > 1000 && flagLongPress == false) {
    flagLongPress = true;
    //Serial.println("LONG PRESS");
    //Serial.println(millis() - timePress);
  }
  
}
// метод проверки ожидание стабильного состояния сигнала
// при нажатой кнопке flagPress= true 
// при отжатой кнопке flagPress= false
// при нажатии на кнопку flagClick= true
void Button::scanState() {

 if ( flagPress != digitalRead(_pin) ) {
     // признак flagPress = текущему состоянию кнопки 
     // (инверсия т.к. активное состояние кнопки LOW) 
     // т.е. состояние кнопки осталось прежним 
     _buttonCount= 0;  // сброс счетчика подтверждений состояния кнопки
  }
  else {
     // признак flagPress не = текущему состоянию кнопки 
     // состояние кнопки изменилось
     _buttonCount++;   // +1 к счетчику состояния кнопки

     if ( _buttonCount >= _timeButton ) {
      // состояние кнопки не мянялось в течение заданного времени
      // состояние кнопки стало устойчивым
      flagPress= ! flagPress; // инверсия признака состояния
     _buttonCount= 0;  // сброс счетчика подтверждений состояния кнопки

      if ( flagPress == true ) flagClick= true; // признак фронта кнопки на нажатие      
     }    
  }
}
// метод установки номера вывода и времени подтверждения
void Button::setPinTime(uint8_t pin, uint8_t timeButton)  {

  _pin= pin;
  _timeButton= timeButton;
  pinMode(_pin, INPUT_PULLUP);  // определяем вывод как вход
}

// описание конструктора класса Button
Button::Button(uint8_t pin, uint8_t timeButton) {

  _pin= pin;
  _timeButton= timeButton;
  pinMode(_pin, INPUT_PULLUP);  // определяем вывод как вход
}
