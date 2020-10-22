#include <Arduino.h>
#include <pins.h>
#include <const.h>
#include <tasks.h>


void setup() {

  Serial.begin(115200); //Uart
  while(!Serial);
  
  ledcSetup(1, 10000, 8);
  ledcSetup(2, 10000, 8);
  ledcSetup(3, 10000, 8);
  ledcSetup(4, 10000, 8);

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
  pinMode(Beeper ,OUTPUT);
  
  if(ota){
     xTaskCreate(
                    ServerOTA,          
                    "ServerOTA",        
                    8000,            
                    NULL,             
                    1,               
                    NULL);

      xTaskCreate(
                    LightCtrlTask,          /* Task function. */
                    "Light",        /* String with name of task. */
                    5000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

  xTaskCreate(
                    CompCtrlTask,          /* Task function. */
                    "CompCtrl",        /* String with name of task. */
                    3000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

  xTaskCreate(
                    FanCtrlTask,          /* Task function. */
                    "FanCtrl",        /* String with name of task. */
                    3000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

  xTaskCreate(
                    HeaterCtrlTask,          /* Task function. */
                    "HeaterCtrl",        /* String with name of task. */
                    1000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

  xTaskCreate(
                    RtcTask,          
                    "RTC",        
                    1000,           
                    NULL,             
                    1,                
                    NULL);

      xTaskCreate(
                    Wdt,          /* Task function. */
                    "Wdt",        /* String with name of task. */
                    1000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

    xTaskCreate(
                    StaticTask,          /* Task function. */
                    "Static",        /* String with name of task. */
                    1000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */
  }

  xTaskCreate(
                    TempRead,          
                    "TempRead",        
                    8000,            
                    NULL,             
                    1,               
                    NULL);           
  
   xTaskCreatePinnedToCore(
    DisplayTask,             /* Task function. */
    "DisplayTask",           /* String with name of task. */
    10000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    1,                /* Priority of the task. */
    NULL,           /* Task handle. */
    1);               /* Core 1 */ 

    
}

void loop() {
   vTaskDelete( NULL );
}