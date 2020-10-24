#include <Arduino.h>

/////////////////////// Setup Config  //////////////////////

#define  language_RU    //Russian 
//#define language_EU   //English

#define  DualZone        //Dual Zone Wine Fridges

//////////////////////  Config End    //////////////////////


//#define  language_RU 
#define language_EU

#ifdef language_EU
  #define text_1    "Settings"
  #define text_2    "Auto Light: "
  #define text_3    "Auto Fan: "
  #define text_4    "Temp: "
  #define text_5    "???"
  #define text_6    "Wireless: "
  #define text_7    "Perf: "
  #define text_8    "On"
  #define text_9    "Off"
  #define text_10   "Display Brt"
  #define text_11   "<Back"
  #define text_12   "Time"
  #define text_13   "Time"
  #define text_14   "Date"
  #define text_15   "Fan control"
  #define text_16   "Light control"
  #define text_17   "Max spd"
  #define text_18   "Max brt"
  #define text_19   "Eco"
  #define text_20   "Balanced"
  #define text_21   "High"
  #define text_22   "Celsius"
  #define text_23   "Fahrenheit"
#endif

#ifdef language_RU
  #define text_1    "Настройки"
  #define text_2    "Авто: "
  #define text_3    "Авто: "
  #define text_4    "Шкала: "
  #define text_5    "???"
  #define text_6    "Сеть: "
  #define text_7    "Режим: "
  #define text_8    "Вкл"
  #define text_9    "Выкл"
  #define text_10   "Яркость"
  #define text_11   "<Назад"
  #define text_12   "Время"
  #define text_13   "Время"
  #define text_14   "Дата"
  #define text_15   "Верниляторы" 
  #define text_16   "Подсветка"
  #define text_17   "Обороты"
  #define text_18   "Яркость"
  #define text_19   "Эко"
  #define text_20   "Средний"
  #define text_21   "Высокий"
  #define text_22   "Цельсия"
  #define text_23   "Фаренгейта"
#endif

#ifdef DualZone
  bool DualZoneMode = true;
#endif

#ifndef DualZone
  bool DualZoneMode = false;
#endif

