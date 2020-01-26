#include <Arduino.h>

//PWM setup
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 10; //Resolution 8, 10, 12, 15

//Light
uint8_t fadeAmount = 1;    // how many points to fade the LED by
uint8_t brightness = 0;
uint8_t briR = 0;
uint8_t briG = 0;
uint8_t briB = 0;
uint8_t briW = 0;

//boot
bool ota = false;
bool ts = false;

//DS18B20 setup
const int oneWireBus = DS18B20;
uint8_t numberOfDevices = 0; // Number of temperature devices found
float tempC[5];

uint8_t heatter_n = 0;
uint8_t zone_1_1_n = 0;
uint8_t zone_1_2_n = 0;
uint8_t zone_2_1_n = 0;
uint8_t zone_2_2_n = 0;

//RTC
int h_rtc = 0;
int min_rtc = 0;
int sec_rtc = 0;
int d_rtc = 0;
int m_rtc = 0;
int y_rtc = 0;
double temp_rtc = 0;

//i2c
bool i2c = false;


//WDT
const uint16_t wdtTimeout = 5000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

//Interrupt
bool button = false;

//WIFI
char* ssid = "Psin(a)";
char* password = "Hey!28R0om!";
char* host = "esp32"; 

//eeprom
#define EEPROM_SIZE 64
const int ota_e = 0;
const int ts_e = 1;
const int heatter_e = 2;
const int zone_1_1_e = 3;
const int zone_1_2_e = 4;
const int zone_2_1_e = 5;
const int zone_2_2_e = 6;
const int numberOfDevices_e = 7;
const int bssid_e = 8;
const int pass_e = 9;

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
bool fade = false;
bool march = false;

String err_str = "";
uint8_t def_time = 6;
uint8_t err_delay = def_time;