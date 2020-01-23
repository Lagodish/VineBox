#include <pins.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "BluetoothSerial.h"
#include <RtcDS3231.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

WebServer server(80);

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define U8LOG_WIDTH 20
#define U8LOG_HEIGHT 8
uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT];
U8G2LOG u8g2log;

RtcDS3231<TwoWire> Rtc(Wire);

void IRAM_ATTR resetModule() {
 // ets_printf("reboot\n");
  esp_restart();
}

void Light( void * parameter )
{
        Serial.println("Light");
        /*
           ledcAttachPin(GP3, 0);
   ledcAttachPin(R, 1);
   ledcAttachPin(G, 2);
   ledcAttachPin(B, 3);
   ledcAttachPin(W, 4);
        */

    while(1){

        ledcWrite(3, briB);
        ledcWrite(1, briR);
        ledcWrite(2, briG);
        ledcWrite(4, briW);

        vTaskDelay(100);    

    }

    Serial.println("Ending Light");
    vTaskDelete( NULL );
 
}
 
void TempRead( void * parameter)
{
    Serial.println("TempRead");
    // Setup a oneWire instance to communicate with any OneWire devices
    OneWire oneWire(oneWireBus);
    // Pass our oneWire reference to Dallas Temperature sensor 
    DallasTemperature sensors(&oneWire);
    // We'll use this variable to store a found device address
    DeviceAddress tempDeviceAddress; 
    // Start the DS18B20 sensor
    sensors.begin();
    vTaskDelay(100);
    numberOfDevices = 1;//sensors.getDeviceCount()+1;
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  
    while(1){
sensors.requestTemperatures();
    for(int i=0;i<numberOfDevices; i++){
        tempC[i]=0;
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){
      // Print the data
      tempC[i] = sensors.getTempC(tempDeviceAddress);
      Serial.print("Temp[");
      Serial.print(i);
      Serial.print("]: ");
      Serial.print(tempC[i]);
      Serial.print(" C");
      Serial.println("");
     // Serial.print(" Temp F: ");
     // Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
    }

        
  }
    vTaskDelay(5000);
        
    }

    Serial.println("Ending TempRead");
    vTaskDelete( NULL );
}

void DisplayOut( void * parameter)
{
    Serial.println("DisplayOut");
    while(1){
        Serial1.println("Test Display");
        vTaskDelay(1000);
    }

    Serial.println("Ending DisplayOut");
    vTaskDelete( NULL );
}


void OLED( void * parameter)
{
    Serial.println("OLED");
    if(i2c){
        while(i2c){
            vTaskDelay(10);
        }
    }
    i2c = true;

      u8g2.begin();  
      u8g2.clear();
  u8g2.setFont(u8g2_font_synchronizer_nbp_tf);	// set the font for the terminal window
  u8g2log.begin(u8g2, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
  u8g2log.setLineHeightOffset(0);	// set extra space between lines in pixel, this can be negative
  u8g2log.setRedrawMode(1);		// 0: Update screen with newline, 1: Update screen for every char  

    i2c = false;
   
    while(1){
     //   Serial.println("OLED");
    if(i2c){
        while(i2c){
            vTaskDelay(10);
        }
    }
    i2c = true;


  char c;
  while (Serial.available() > 0) {
    c = Serial.read();			// read from Serial Monitor
    if(i2c){
        while(i2c){
            vTaskDelay(2);
        }
    }
    i2c = true;
    u8g2log.print(c);               // print to display
    i2c = false;
  //  Serial.print(c);                // and print back to monitor
  }
       // Serial1.println("Test olED");
        i2c = false;
        vTaskDelay(100);
    }

    Serial.println("Ending OLED");
    vTaskDelete( NULL );
}

void set_time(int h,int min,int s,int y,int m, int d){
      //  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date &amp; time, for example to set
    // January 21, 2014 at 3am you would call:
      //   rtc.adjust(DateTime(y, m, d, h, min, s));
//TODO  

}

void BLE( void * parameter)
{
    
    uint64_t chipid; 
    chipid=ESP.getEfuseMac();
    Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(chipid>>32));

    BluetoothSerial ESP_BT; //Object for Bluetooth
    //ESP_BT.esp_ble_power_type_t(9);
    //ESP_BT.esp_power_level_t(7);
  //  ESP_BT.esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
    ESP_BT.begin("VineBox_" + String((uint16_t)(chipid>>32))); //Name of your Bluetooth Signal
    ESP_BT.println("BLE works!");
    while(1){ 

    if (ESP_BT.available()) {
        String str = ESP_BT.readString();
        Serial.print(str);
        u8g2log.print(str);
            
    if(str.equals("help\r\n")||str.equals("h\r\n")||str.equals("Help\r\n")){
        ESP_BT.println("Available commands help/h, set/s, flags/f, time, temp/t, light/l reboot"); 
    }

    if(str.equals("Light\r\n")||str.equals("l\r\n")||str.equals("light\r\n")){

        ESP_BT.print("Please write r,g,b,w, one by one. (0-255)");
        int val[4];

        for(int i=0; i < 4; i++){
        val[i]=0;
        while (!ESP_BT.available())
        {
         vTaskDelay(10);   
        }
                       
        val[i]=ESP_BT.parseInt();

        if(val[i]==0){
            i--;
            continue;
        }
        if(val[i]>255){
            val[i]=255;
        }
        if(val[i]<0){
            val[i]=0;
        }

        }

        briR = val[0];
        briG = val[1];
        briB = val[2];
        briW = val[3];

        ESP_BT.println("OK!"); 
    }

    if(str.equals("temp\r\n")||str.equals("t\r\n")){
        if(numberOfDevices==0){
            ESP_BT.print("No devices found!");
        }
        else{
            for(int i=0;i<numberOfDevices; i++)
                {
                ESP_BT.print("Temp[");
                ESP_BT.print(i);
                ESP_BT.print("]: ");
                ESP_BT.print(tempC[i]);
                ESP_BT.print(" C");
                ESP_BT.println("");
                if(Rtc.IsDateTimeValid()){
                    ESP_BT.printf("Temp[RTC]: %.2f C", temp_rtc);
                }
                }
    
            }
            }
    if(str.equals("flags\r\n")||str.equals("f\r\n")){

        ESP_BT.print("Flags: err[");
        ESP_BT.print(err_flag);
        ESP_BT.print("],light[");
        ESP_BT.print(light_flag);
        ESP_BT.print("],heater[");
        ESP_BT.print(heater_flag);
        ESP_BT.print("],comp[");
        ESP_BT.print(comp_flag);
        ESP_BT.print("],rgb[");
        ESP_BT.print(rgb_flag);
        ESP_BT.print("],fan1[");
        ESP_BT.print(fan1_flag);
        ESP_BT.print("],fan2[");
        ESP_BT.print(fan2_flag);
        ESP_BT.print("],beeper[");
        ESP_BT.print(beeper_flag);
        ESP_BT.print("]");
        ESP_BT.println("");
    }

    if(str.equals("time\r\n")||str.equals("Time\r\n")||str.equals("Time -i\r\n")||str.equals("time -i\r\n"))
    {
        if(str.equals("Time -i\r\n")||str.equals("time -i\r\n")){
            
        ESP_BT.print("Time: ");
        ESP_BT.print(h_rtc);
        ESP_BT.print(":");
        ESP_BT.print(min_rtc);
        ESP_BT.print(":");
        ESP_BT.print(sec_rtc);
        ESP_BT.println("");

        }
        else{
        //str.equalsIgnoreCase("*-r*")''
        if((h_rtc>24)||(min_rtc>59)||(sec_rtc>59)||(d_rtc>33)||(m_rtc>12)){
            ESP_BT.print("RTC Err!");//(h_rtc>24)||(min_rtc>59)||(sec_rtc>59)||(d_rtc>33)||(m_rtc>13)
        }
        else{
        ESP_BT.print("Time: ");
        ESP_BT.print(h_rtc);
        ESP_BT.print(":");
        ESP_BT.print(min_rtc);
        ESP_BT.print(":");
        ESP_BT.print(sec_rtc);
        ESP_BT.println("");
        }
        }
    }

    if(str.equals("TimeSet\r\n")||str.equals("ts\r\n"))
    {
        set_t = true;
    }

    if(str.equals("set\r\n")||str.equals("s\r\n"))
    {
        if (!Rtc.IsDateTimeValid()) 
        {
            ESP_BT.println("Couldn't find RTC");
            err_flag = true;
            err_str += "RTC, ";
        }
        else
        {
            ESP_BT.print("Please write h,m,s,y,m,d one by one");
            int set[6];
            for(int i=0; i< 6; i++){

                while (!ESP_BT.available())
                {
                    vTaskDelay(10);   
                }
                       
            set[i]=ESP_BT.parseInt();

            if(set[i]==0){
                i--;
                continue;
            }
        }
        
        ESP_BT.printf("Time: %d:%d:%d %d.%d.%d", set[0],set[1],set[2],set[3],set[4],set[5]);
        ESP_BT.println("Correct? Y/n");
        bool test = true;
        while (test)
        {           
            if (ESP_BT.available())
           {         
         String str = ESP_BT.readString();

         if(str.equals("Y\r\n")||str.equals("y\r\n")){
                set_time(set[0],set[1],set[2],set[3],set[4],set[5]);
                ESP_BT.println("Setted!");
                test = false;
          }
          else if(str.equals("N\r\n")||str.equals("n\r\n")){
            ESP_BT.println("Aborted!");
            test = false;
          }        
        }
        else{
            vTaskDelay(10);  
        }
        }              
     }       
    }

    if(str.equals("reboot\r\n")||str.equals("Reboot\r\n")){
            ESP_BT.println("Reboot!");
            vTaskDelay(500);
            resetModule();        
    }
    
                                                                 
  }
        vTaskDelay(300);
    }
    
    Serial.println("Ending BLE");
    vTaskDelete( NULL );
}

void CompCtrl( void * parameter)
{
    Serial.println("CompCtrl");
    while(1){
    vTaskDelay(1000);

    }

    Serial.println("Ending CompCtrl");
    vTaskDelete( NULL );
}

void FanCtrl( void * parameter)
{
    Serial.println("FanCtrl");
    while(1){
    vTaskDelay(1000);
        
    }

    Serial.println("Ending FanCtrl");
    vTaskDelete( NULL );
}

void HeaterCtrl( void * parameter)
{
    Serial.println("HeaterCtrl");
    while(1){
    vTaskDelay(1000);

    //Temp PI regulator 
        
    }

    Serial.println("Ending HeaterCtrl");
    vTaskDelete( NULL );
}

void RTC( void * parameter)
{
    Serial.println("RTC");
        if(i2c){
        while(i2c){
            vTaskDelay(10);
        }
    }
    int counter = 0;
    i2c = true;
      
     Rtc.Begin();
    Serial.println();
        if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

        Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    i2c = false;
  //  Rtc.SetDateTime(compiled);
    while(1){
        if(i2c){
        while(i2c){
            vTaskDelay(10);
        }
        }
        i2c = true;
    if(set_t){
            Rtc.SetDateTime(compiled);
            set_t = false;
        }


    if(!Rtc.IsDateTimeValid()){
            if(counter==0)
                Serial.println("RTC ERR, lets set the time!");
            if(counter>20)
                counter = 0;
            counter++;
         err_flag = true;
       //  err_str += "RTC, ";

    }
    else{

    RtcDateTime now = Rtc.GetDateTime();
	RtcTemperature temp = Rtc.GetTemperature();


    h_rtc = now.Hour();
    min_rtc = now.Minute();
    sec_rtc = now.Second();
    d_rtc = now.Day();
    m_rtc = now.Month();
    y_rtc = now.Year();
    temp_rtc = double( temp.AsFloatDegC() );

    err_flag = false;
    }
    i2c = false;
    vTaskDelay(900);
        
    }

    Serial.println("Ending RTC");
    vTaskDelete( NULL );
}


void Wdt( void * parameter)
{
    Serial.println("Wdt");

    timer = timerBegin(0, 80, true);                  //timer 0, div 80
    timerAttachInterrupt(timer, &resetModule, true);  //attach callback
    timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
    timerAlarmEnable(timer);                          //enable interrupt

    while(1){

    timerWrite(timer, 0); //reset timer (feed watchdog)
    vTaskDelay(100);
        
    }

    Serial.println("Ending Wdt");
    vTaskDelete( NULL );
}

void Static( void * parameter)
{
    Serial.println("Static");
    uint16_t old_Value=0; 
    
    while(1){

    if(err_flag){
        err_delay = 1;
    }
    else{
        err_delay = 6;
    }

    if(old_Value != intTriggerCount){
        old_Value = intTriggerCount; // something to compare against
        Serial.println("Int!");
    }

    vTaskDelay(1000);
    
    }

    Serial.println("Ending Static");
    vTaskDelete( NULL );
}

void led( void * parameter)
{
    Serial.println("led");

    while(1){

        ledcWrite(0, brightness);
  brightness = brightness + fadeAmount; 
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }

 vTaskDelay(err_delay);    

    }

    Serial.println("Ending led");
    vTaskDelete( NULL );
}


void ServerOTA( void * parameter)
{
    Serial.println("ServerOTA");
WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      vTaskDelay(1000); 
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
    while(1){
server.handleClient();
 vTaskDelay(10);    

    }

    Serial.println("Ending ServerOTA");
    vTaskDelete( NULL );
}