#include <Arduino.h>

//PWM setup
const int freq = 10000;
const uint8_t resolution = 8;

//Light
uint8_t briR = 0;
uint8_t briG = 0;
uint8_t briB = 0;
uint8_t briW = 0;

//Display
const char DEGREE_SYMBOL[] = { 0xB0, '\0' };
const char ALERT_SYMBOL[] = { 71, '\0' }; //DOTO как в Aplle
const char wifi_SYMBOL[] = { 80, '\0' }; //Заменить значок (не вытянутый)
const char setup_SYMBOL[] = { 66, '\0' };

//boot
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
const char* ssid = "";
const char* password = "";

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

bool LightCtrl=LOW;
bool RGBCtrl=LOW;
bool FanCtrl=LOW;
bool Temp_mode=HIGH;
bool mainScreenOn = true;
bool showTemp = false;
bool Wireless=false;
bool AutoBRT=false;

bool butt1_l = false;
bool butt2_l = false;
bool butt3_l = false;
bool butt4_l = false;

uint8_t PERF=0;
uint8_t BRT_max = 0;
uint8_t RGB_set = 0;
uint8_t SPD_max = 0;
uint8_t BRT_Disp = 0;
uint8_t timer_1 = 0;
uint8_t blink=0;
double setted_temp = 0.0;
double temp = 0.0;
double tempPrint =0.0;