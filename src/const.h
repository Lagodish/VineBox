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
uint8_t h_rtc = 0;
uint8_t min_rtc = 0;
uint8_t sec_rtc = 0;
uint8_t d_rtc = 0;
uint8_t m_rtc = 0;
uint8_t y_rtc = 0;
bool set_t = false;
double temp_rtc = 0;

//oled
//#define SCREEN_WIDTH 128 // OLED display width, in pixels
//#define SCREEN_HEIGHT 64 // OLED display height, in pixels
bool i2c = false;
//#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)


//WDT
const uint8_t wdtTimeout = 5000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

//Interrupt
uint16_t intTriggerCount=0;
bool button = false;
//WIFI
const char* ssid = "Psin(a)";
const char* password = "Hey!28R0om!";
const char* host = "esp32";

//eeprom
uint8_t addr = 0;
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
String err_str = "";
bool fade = false;
uint8_t def_time = 6;
uint8_t err_delay = def_time;
//err_str += "RTC, ";