#include <Arduino.h>
#include <pins.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Preferences.h> //TODO NVS
Preferences preferences;
#include <display.h>

SemaphoreHandle_t i2c_mutex;

void IRAM_ATTR resetModule(){esp_restart();}

void LightCtrlTask( void * parameter )
{
    Serial.println("Light");

    while(1){
    ledcWrite(3, 0); //briB
    ledcWrite(1, 0); //briR
    ledcWrite(2, 0); //briG
    ledcWrite(4, 0); //brt

    vTaskDelay(1000/portTICK_PERIOD_MS);
    }

    Serial.println("Ending Light");
    vTaskDelete( NULL );
}

void DataStorage( void * parameter)
{
    Serial.println("DataStorage");
    preferences.begin("VineBoxData", false);
    //preferences.clear(); // Удаляем все настройки под заданным пространством имён
    //preferences.remove("counter"); // Удаляем отдельный ключ
    if(preferences.getBool("StartVB", false)==false){
        preferences.putBool("StartVB",true);
        preferences.putUInt("BRT_Disp",50);
        preferences.putUInt("BRT_max",80);
        preferences.putUInt("SPD_max",80);
        preferences.putUInt("PERF",1);
        preferences.putUInt("Wireless",0);
        preferences.putDouble("setted_temp",16);
        preferences.putBool("Temp_mode",HIGH);
        preferences.putBool("LightCtrl",HIGH);
        preferences.putBool("FanCtrl",HIGH);
    }
    BRT_Disp = preferences.getUInt("BRT_Disp");
    BRT_max = preferences.getUInt("BRT_max");
    SPD_max = preferences.getUInt("SPD_max");
    PERF = preferences.getUInt("PERF");
    Wireless = preferences.getUInt("Wireless");
    setted_temp = preferences.getDouble("setted_temp");
    Temp_mode = preferences.getBool("Temp_mode");
    LightCtrl = preferences.getBool("LightCtrl");
    FanCtrl = preferences.getBool("FanCtrl");

    preferences.end();
    delay(10);
    Serial.println("Ending DataStorage");
    vTaskDelete( NULL );
}

void TempRead( void * parameter)
{
    Serial.println("TempRead");
    OneWire oneWire(oneWireBus);// Setup a oneWire instance to communicate with any OneWire devices
    DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature sensor 
    DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address
    sensors.begin();// Start the DS18B20 sensor
  
    while(1){
    sensors.requestTemperatures();
    for(int i=0;i<=numberOfDevices; i++){

    if(sensors.getAddress(tempDeviceAddress, i)){// Search the wire for address
      tempC[i] = sensors.getTempC(tempDeviceAddress);// Print the data
      //Serial.print("Temp[");
      //Serial.print(i);
      //Serial.print("]: ");
      //Serial.print(tempC[i]);
      //Serial.print(" C");
      //Serial.println("");
      //temp_cache=temp_cache+tempC[i];
     // Serial.print(" Temp F: ");
     // Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
    }}
    vTaskDelay(1000/portTICK_PERIOD_MS);
    }

    Serial.println("Ending TempRead");
    vTaskDelete( NULL );
}

void CompCtrlTask( void * parameter)
{
    Serial.println("CompCtrl");
    while(temp_cache<5||temp_cache>64){
    vTaskDelay(1000/portTICK_PERIOD_MS);
    }

    while(1){
    digitalWrite(Comp,Hysteresis(temp_cache));//inveted pin
    for(int i=0;i<180;i++){
    vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    }

    Serial.println("Ending CompCtrl");
    vTaskDelete( NULL );
}

void FanCtrlTask( void * parameter)
{
    Serial.println("FanCtrl");
    while(1){
    vTaskDelay(1000/portTICK_PERIOD_MS);        
    }

    Serial.println("Ending FanCtrl");
    vTaskDelete( NULL );
}

void HeaterCtrlTask( void * parameter)
{
    Serial.println("HeaterCtrl");
    while(1){
    vTaskDelay(1000/portTICK_PERIOD_MS);
    }

    Serial.println("Ending HeaterCtrl");
    vTaskDelete( NULL );
}

void RtcTask( void * parameter)
{
    Serial.println("RTC");
    xSemaphoreTake(i2c_mutex, portMAX_DELAY);
    
    Rtc.Begin();
    if (!Rtc.GetIsRunning()){
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);

    //RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

    //Rtc.SetDateTime(compiled);
    xSemaphoreGive(i2c_mutex);

    while(1){
    xSemaphoreTake(i2c_mutex, portMAX_DELAY);

    if(!Rtc.IsDateTimeValid()){
    Serial.println("RTC ERR, lets set the time!");
    //Rtc.SetDateTime(compiled);
    err_flag = true;}
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
    if(y_rtc<1000){mainMenu[0].enabled=disabledStatus; }
    else{mainMenu[0].enabled=enabledStatus;}
    err_flag = false;
    }
    xSemaphoreGive(i2c_mutex);
    for(int i=0; i< 30; i++){
    vTaskDelay(1000/portTICK_PERIOD_MS);
    }
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
    vTaskDelay(1000/portTICK_PERIOD_MS);        
    }

    Serial.println("Ending Wdt");
    vTaskDelete( NULL );
}

void StaticTask( void * parameter)
{
    Serial.println("Static");

    while(1){
    vTaskDelay(1000/portTICK_PERIOD_MS);
    }

    Serial.println("Ending Static");
    vTaskDelete( NULL );
}

void DisplayTask( void * parameter)
{
    Serial.println("Display");
    xSemaphoreTake(i2c_mutex, portMAX_DELAY);
    
    Wire.begin();
    u8g2.begin();
    u8g2.enableUTF8Print();	
    u8g2.setFont(fontName);
    // u8g2.setBitmapMode(0);
    mainMenu[0].enabled=disabledStatus;
    nav.idleTask=MainScreen;//point a function to be used when menu is suspended
    nav.timeOut=30;
    nav.idleOn(MainScreen);

    xSemaphoreGive(i2c_mutex);
   
    while(1){       
    butt1.tick();
    butt2.tick();
    butt3.tick();
    butt4.tick();
    if(tempC[numberOfDevices]<64&&tempC[numberOfDevices]>0){
    temp_cache=tempC[numberOfDevices];
    }

    if (butt1.isClick()){butt1_l = true;nav.doNav(enterCmd);Serial.println("Enter");}else{butt1_l = false;}
    if (butt2.isClick()){butt2_l = true;nav.doNav(upCmd);Serial.println("Up");}else{butt2_l = false;}
    if (butt3.isClick()){butt3_l = true;nav.doNav(downCmd);Serial.println("Down");} else{butt3_l = false;}
    if (butt4.isClick()){butt4_l = true;nav.doNav(escCmd);Serial.println("Esc");} else{butt4_l = false;}
    //if(butt1_l||butt2_l||butt3_l||butt4_l){beep();}
    if(mainScreenOn&&(butt2_l||butt2.isStep())){
    if(Temp_mode){
    setted_temp+=0.5;
    if(setted_temp>18){setted_temp=18;}}
    else{
    setted_temp+=1;
    if(setted_temp>64){setted_temp=64;}}
    showTemp=true;timer_1=1;}

    if(mainScreenOn&&(butt3_l||butt3.isStep())){if(Temp_mode){
    setted_temp-=0.5;
    if(setted_temp<5){setted_temp=5;}}
    else{
    setted_temp-=1;
    if(setted_temp<41){setted_temp=41;}}
    showTemp=true;timer_1=1;}

    xSemaphoreTake(i2c_mutex, portMAX_DELAY);
    nav.doInput();
    //if (nav.changed(0))
    int contrast = map(BRT_Disp, 0, 100, 0, 190);
    u8g2.setContrast(contrast);
    
    u8g2.firstPage();
    do nav.doOutput(); while(u8g2.nextPage());

    xSemaphoreGive(i2c_mutex);

    if(showTemp){timer_1++;if(timer_1>101){timer_1=0;showTemp=false; writeTemp();}}
    else{timer_1=0;showTemp=false;}
    vTaskDelay(30/portTICK_PERIOD_MS);
    }

    Serial.println("Ending Display");
    vTaskDelete( NULL );
}


void ServerOTA( void * parameter)
{
  Serial.println("ServerOTA");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
    //WiFi.setHostname("VineBox");
  //  tcpip_adapter_set_hostname
    
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(1000);
        Serial.print(".");
    }

  // Port defaults to 3232
   ArduinoOTA.setPort(3232);

   // char* name = "VineBox_" + String((uint16_t)(chipid>>32));
  // Hostname defaults to esp3232-[MAC]
   ArduinoOTA.setHostname("VineBoxOTA");
   ArduinoOTA.setPassword("quZJU4KNywpHm9pS");

    //Sets the password as above but in the form MD5(password). Default NULL
    //ArduinoOTA.setPasswordHash(md5_);

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {    
      Serial.println("End");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r\n", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

    while(1){
    ArduinoOTA.handle();
    vTaskDelay(5000/portTICK_PERIOD_MS);
    }

    Serial.println("Ending ServerOTA");
    vTaskDelete( NULL );
}

/*
void BLE( void * parameter)
{
    uint64_t chipid; 
    chipid=ESP.getEfuseMac();
    Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(chipid>>32));

    BluetoothSerial ESP_BT; //Object for Bluetooth
    //ESP_BT.esp_ble_power_type_t(9);
    //ESP_BT.esp_power_level_t(7);
    //ESP_BT.esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
    ESP_BT.begin("VineBox_" + String((uint16_t)(chipid>>32))); //Name of your Bluetooth Signal
    ESP_BT.println("BLE works!");


    while(1){ 
    if (ESP_BT.available()) {
        
        String str = ESP_BT.readString();
        Serial.print(str);
        //u8g2log.print(str);
    
    if(str.equals("help\r\n")||str.equals("h\r\n")||str.equals("Help\r\n")){
        ESP_BT.println("Available commands: \r\nhelp/h (-a), \r\nset/s, \r\nflags/f, \r\ntime, \r\ntemp/t,\r\nlight/l,\r\nota -on/off,\r\n reboot"); 
    }

    if(str.equals("help -a\r\n")||str.equals("h -a\r\n")||str.equals("Help -a\r\n")){
        ESP_BT.println("More cmnd: \r\nerr_flag, \r\nds18, \r\nds18_set, \r\nwifi, \r\nflag_reset, \r\nmax_br, \r\nmin_br, \r\nl, beep \r\n"); //TODO lag_rest
    }

    if(str.equals("beep\r\n")){
        ESP_BT.println("OK!"); 
      //  march = true;
    }

    if(str.equals("Ds18_set\r\n")||str.equals("ds18_set\r\n")){
                //numberOfDevices
        ESP_BT.println("With heatter? Y/n"); //TODO
        bool test = true;
        while (test)
        {           
            if (ESP_BT.available())
           {         
         String str = ESP_BT.readString();

         if(str.equals("Y\r\n")||str.equals("y\r\n")){
                ESP_BT.println("With heatter");
                test = false;
          }
          else if(str.equals("N\r\n")||str.equals("n\r\n")){
            ESP_BT.println("Without heatter!");
            test = false;
          }        
        }
             //......................................
    }
    }

        if(str.equals("Fade\r\n")||str.equals("fade\r\n")){
        fade = !fade;
           }
 
        if(str.equals("Wifi\r\n")||str.equals("wifi\r\n")){
        ESP_BT.println("With heatter? Y/n"); //TODO
        ESP_BT.println("Please write your WIFI name, pass"); //TODO 
        // EEPROM.writeString(bssid_e/pass_e, gratitude); 
        //......................................
           }

        if(str.equals("ds18\r\n")||str.equals("ds18b20\r\n")||str.equals("Ds18\r\n")||str.equals("Ds18b20\r\n")){
         int val[1];
        ESP_BT.println("Please wrie numberOfDevices"); 
        for(int i=0; i < 1; i++){
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
        if(val[i]>100){
            val[i]=100;
        }
        if(val[i]<0){
            val[i]=1;
        }


        }
        EEPROM.write(numberOfDevices_e,val[0]); //TODO fix indexs
        EEPROM.commit();
        numberOfDevices=val[0];
        ESP_BT.println("OK!"); 
    }

    if(str.equals("err_flag\r\n")){
        ESP_BT.print("Please write def. delay time (ms). (1-100)");
        int val[1];

        for(int i=0; i < 1; i++){
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
        if(val[i]>100){
            val[i]=100;
        }
        if(val[i]<0){
            val[i]=1;
        }


        }

        def_time=val[0];
        
        }
    

    if(str.equals("Ota -on\r\n")||str.equals("ota -on\r\n")||str.equals("OTA -on\r\n")||str.equals("OTA\r\n")){
        if (EEPROM.begin(EEPROM_SIZE))
            {
            EEPROM.write(ota_e,true);
            EEPROM.commit();
            ESP_BT.println("OTA ON! Reboot..."); 
            vTaskDelay(100);
            resetModule();
        }
        else{
            ESP_BT.println("EEPROM ERROR!"); 
        }
        
    }

        if(str.equals("Ota -off\r\n")||str.equals("ota -off\r\n")||str.equals("OTA -off\r\n")){
        if (EEPROM.begin(EEPROM_SIZE))
            {
            EEPROM.write(ota_e,false);
            EEPROM.commit();
            ESP_BT.println("OTA OFF!"); 
        }
        else{
            ESP_BT.println("EEPROM ERROR!"); 
        }
        
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
                }
                    if(Rtc.IsDateTimeValid()){
                    ESP_BT.printf("Temp[RTC]: %.2f C", temp_rtc);
                    ESP_BT.println("");
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
        if((h_rtc>24)||(min_rtc>59)||(h_rtc<0)||(min_rtc<0)){
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
            ESP_BT.print("Please write h,m one by one");
            int set[2];
            for(int i=0; i< 2; i++){

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
        
        ESP_BT.printf("Time: %d:%d", set[0],set[1]);
        ESP_BT.println("Correct? Y/n");
        bool test = true;
        while (test)
        {           
            if (ESP_BT.available())
           {         
         String str = ESP_BT.readString();

         if(str.equals("Y\r\n")||str.equals("y\r\n")){
                set_time(set[0],set[1]);
                ESP_BT.println("Setted!");
                test = false;
          }
          else if(str.equals("N\r\n")||str.equals("n\r\n")){
            ESP_BT.println("Aborted!");
            test = false;
          }        
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
*/