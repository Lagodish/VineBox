#include <Arduino.h>
#include <pins.h>
#include <const.h>
#include <tasks.h>




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //Uart
  Serial1.begin(9600); //Displayt  
  //Wire.begin(21, 22, 400000); // 21 & 22 are default on ESP32   

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
  
  pinMode(GP1,INPUT_PULLUP);
  pinMode(GP2,INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(GP1),isr_1,FALLING);
  //attachInterrupt(digitalPinToInterrupt(GP2),isr_2,FALLING);

  delay(100);
 
     xTaskCreate(
                    ServerOTA,          
                    "ServerOTA",        
                    8000,            
                    NULL,             
                    1,               
                    NULL);

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
  
  xTaskCreate(
                    BLE,          
                    "BLE",        /* String with name of task. */
                    10000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

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