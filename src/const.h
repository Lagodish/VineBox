#include <Arduino.h>


//PWM setup
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 10; //Resolution 8, 10, 12, 15

//Light
int fadeAmount = 1;    // how many points to fade the LED by
int brightness = 0;
int briR = 0;
int briG = 0;
int briB = 0;
int briW = 0;



//DS18B20 setup
const int oneWireBus = DS18B20;
int numberOfDevices = 0; // Number of temperature devices found
float tempC[5];

//RTC
int h_rtc = 0;
int min_rtc = 0;
int sec_rtc = 0;

//WDT
const int wdtTimeout = 3000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

//Interrupt
uint16_t intTriggerCount=0;

//WIFI
const char* ssid = "Psin(a)";
const char* password = "Hey!28R0om!";

// Flags
bool light_flag = true;
bool heater_flag = false;
bool comp_flag = false;
bool rgb_flag = false;
bool fan1_flag = false;
bool fan2_flag = false;
bool beeper_flag = true;
bool err_flag = false;
String err_str = "";
//err_str += "RTC, ";