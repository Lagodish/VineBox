#include <Arduino.h>

//PWM setup
const int freq = 10000;
const uint8_t ledChannel = 0;
const uint8_t resolution = 10; //Resolution 8, 10, 12, 15

//Light
uint8_t briR = 0;
uint8_t briG = 0;
uint8_t briB = 0;
uint8_t briW = 0;

//boot
bool ota = false;
bool ts = false;

//DS18B20 setup
const int oneWireBus = DS18B20;
uint8_t numberOfDevices = 0; 
float tempC[5];
double temp_cache = 0.0;

//RTC
uint8_t h_rtc = 0;
uint8_t min_rtc = 0;
uint8_t sec_rtc = 0;
uint8_t d_rtc = 0;
uint8_t m_rtc = 0;
uint16_t y_rtc = 0;
double temp_rtc = 0;

//WDT
const uint16_t wdtTimeout = 3000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

//WIFI
const char* ssid = "Psin(a)";
const char* password = "Hey!28R0om!";

//eeprom
#define EEPROM_SIZE 64

// Flags
bool light_flag = true;
bool heater_flag = false;
bool comp_flag = false;
bool rgb_flag = false;
bool fan1_flag = false;
bool fan2_flag = false;
bool beeper_flag = true;
bool err_flag = false;
bool set_t = false;
