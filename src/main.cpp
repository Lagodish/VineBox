#include <Arduino.h>
#include <pins.h>
#include <const.h>
#include <tasks.h>

void setup() {

   Serial.begin(9600); //Uart
   while(!Serial);
   i2c_mutex = xSemaphoreCreateMutex();
   antenna_mutex = xSemaphoreCreateMutex();

   ledcSetup(1, freq, resolution);
   ledcSetup(2, freq, resolution);
   ledcSetup(3, freq, resolution);
   ledcSetup(4, freq, resolution);

   ledcAttachPin(R, 1);
   ledcAttachPin(G, 2);
   ledcAttachPin(B, 3);
   ledcAttachPin(W, 4);

   pinMode(F1 ,OUTPUT);       digitalWrite(F1,LOW);
   pinMode(F2 ,OUTPUT);       digitalWrite(F2,LOW);
   pinMode(Comp ,OUTPUT);     digitalWrite(Comp,LOW);
 
   if(ota){
   xTaskCreate(
      ServerOTA,          
      "ServerOTA",        
      8000,            
      NULL,             /* Parameter passed as input of the task */          
      1,                /* Priority of the task. */           
      NULL);}            /* Task handle. */
   
   xTaskCreate(
      DataStorage,
      "DataStorage",
      5000,
      NULL,             /* Parameter passed as input of the task */
      2,                /* Priority of the task. */
      NULL);            /* Task handle. */

   delay(100);

   xTaskCreate(
      LightCtrlTask,
      "Light",
      5000,
      NULL,             /* Parameter passed as input of the task */
      1,                /* Priority of the task. */
      NULL);            /* Task handle. */

   xTaskCreate(
      CompCtrlTask,
      "CompCtrl",
      3000,
      NULL,             /* Parameter passed as input of the task */
      1,                /* Priority of the task. */
      NULL);            /* Task handle. */

   xTaskCreate(
      FanCtrlTask,
      "FanCtrl",
      3000,
      NULL,             /* Parameter passed as input of the task */
      1,                /* Priority of the task. */
      NULL);            /* Task handle. */

   xTaskCreate(
      HeaterCtrlTask,
      "HeaterCtrl",
      1000,
      NULL,             /* Parameter passed as input of the task */
      1,                /* Priority of the task. */
      NULL);            /* Task handle. */

   xTaskCreate(
      RtcTask,          
      "RTC",        
      1000,           
      NULL,             /* Parameter passed as input of the task */         
      1,                /* Priority of the task. */               
      NULL);            /* Task handle. */

   xTaskCreate(
      Wdt,
      "Wdt",
      1000,
      NULL,             /* Parameter passed as input of the task */
      1,                /* Priority of the task. */
      NULL);            /* Task handle. */

   xTaskCreate(
      StaticTask,
      "Static",
      1000,
      NULL,             /* Parameter passed as input of the task */
      1,                /* Priority of the task. */
      NULL);            /* Task handle. */
  

   xTaskCreate(
      TempRead,          
      "TempRead",        
      8000,            
      NULL,             /* Parameter passed as input of the task */             
      1,                /* Priority of the task. */        
      NULL);            /* Task handle. */       
   
   delay(500);

   xTaskCreatePinnedToCore(
      DisplayTask,
      "DisplayTask",
      10000,
      NULL,
      1,              /* Priority of the task. */
      NULL,           /* Task handle. */
      0);             /* Core 0 */ 

}

void loop() {
   vTaskDelete( NULL );
}