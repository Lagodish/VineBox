#include <Arduino.h>
#include <pins.h>
#include <const.h>
#include <tasks.h>

void IRAM_ATTR isr() {
  button = true;
}

void setup() {

  Serial.begin(9600); //Uart
  Serial1.begin(9600); //Displayt  

if (EEPROM.begin(EEPROM_SIZE))
  {
    ota = EEPROM.read(ota_e);
    ts = EEPROM.read(ts_e);
    heatter_n = EEPROM.read(heatter_e);
    zone_1_1_n = EEPROM.read(zone_1_1_e);
    zone_1_2_n = EEPROM.read(zone_1_2_e);
    zone_2_1_n = EEPROM.read(zone_2_1_e);
    zone_2_2_n = EEPROM.read(zone_2_2_e);
    briMIN = EEPROM.read(briMIN_e);
    briMAX = EEPROM.read(briMAX_e);
    if(briMIN>briMAX){
      briMAX = 130;
      briMIN = 0;
    }
    numberOfDevices=EEPROM.read(numberOfDevices_e);
    if((numberOfDevices<0)||(numberOfDevices>5)){
      numberOfDevices = 0;
    }
  }

  

  ledcSetup(0, 1000, 8);
  ledcSetup(1, 10000, 8);
  ledcSetup(2, 10000, 8);
  ledcSetup(3, 10000, 8);
  ledcSetup(4, 10000, 8);

  ledcAttachPin(GP3, 0);
  ledcAttachPin(R, 1);
  ledcAttachPin(G, 2);
  ledcAttachPin(B, 3);
  ledcAttachPin(W, 4);

  pinMode(R ,OUTPUT);
  pinMode(G ,OUTPUT);
  pinMode(B ,OUTPUT);
  pinMode(W ,OUTPUT);
  pinMode(F1 ,OUTPUT);
  pinMode(F2 ,OUTPUT);
  pinMode(GP3 ,OUTPUT);
  pinMode(Beeper ,OUTPUT);

  pinMode(GP4 ,INPUT_PULLUP);  
  pinMode(GP1,INPUT_PULLUP);
  pinMode(GP2,INPUT_PULLUP);

  attachInterrupt(GP4, isr, FALLING);

for(int i =0; i< 3000; i++){

  ledcWrite(0, brightness);
  brightness = brightness + fadeAmount; 
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
delayMicroseconds(400);
 //delay(1);  
}
  delay(100);
  
  if(ota){
     xTaskCreate(
                    ServerOTA,          
                    "ServerOTA",        
                    8000,            
                    NULL,             
                    1,               
                    NULL);
  }
  xTaskCreate(
                    TempRead,          
                    "TempRead",        
                    8000,            
                    NULL,             
                    1,               
                    NULL);           
  
  xTaskCreate(
                    OLED,          
                    "OLED",        
                    10000,            
                    NULL,             
                    1,               
                    NULL); 
  
 
  xTaskCreate(
                    DisplayOut,          /* Task function. */
                    "DisplayOut",        /* String with name of task. */
                    10000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */
  if(!ota){
  xTaskCreate(
                    BLE,          
                    "BLE",       
                    10000,           
                    NULL,             
                    1,                
                    NULL);            
  }

  xTaskCreate(
                    Light,          /* Task function. */
                    "Light",        /* String with name of task. */
                    5000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

  xTaskCreate(
                    CompCtrl,          /* Task function. */
                    "CompCtrl",        /* String with name of task. */
                    3000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

  xTaskCreate(
                    FanCtrl,          /* Task function. */
                    "FanCtrl",        /* String with name of task. */
                    3000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

  xTaskCreate(
                    HeaterCtrl,          /* Task function. */
                    "HeaterCtrl",        /* String with name of task. */
                    10000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

  xTaskCreate(
                    RTC,          
                    "RTC",        
                    10000,           
                    NULL,             
                    1,                
                    NULL);

    xTaskCreate(
                    Wdt,          /* Task function. */
                    "Wdt",        /* String with name of task. */
                    4000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

    xTaskCreate(
                    Static,          /* Task function. */
                    "Static",        /* String with name of task. */
                    4000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

    xTaskCreate(
                    led,          /* Task function. */
                    "led",        /* String with name of task. */
                    5000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */
    
}

void loop() {
  // put your main code here, to run repeatedly:
    vTaskDelay(1000);
}