#include <Arduino.h>
#include <pins.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <Preferences.h> //TODO NVS
#include <display.h>

SemaphoreHandle_t i2c_mutex;
SemaphoreHandle_t antenna_mutex;

void IRAM_ATTR resetModule(){esp_restart();}

void setRGB(int color){
    uint8_t _r =0;
    uint8_t _g =0;
    uint8_t _b =0;

    if (color <= 255) {						// красный макс, зелёный растёт
		_r = 255;
		_g = color;
		_b = 0;
	}
	else if (color > 255 && color <= 510) {		// зелёный макс, падает красный 
		_r = 510 - color;
		_g = 255;
		_b = 0;
	}
	else if (color > 510 && color <= 765) {		// зелёный макс, растёт синий
		_r = 0;
		_g = 255;
		_b = color - 510;
	}
	else if (color > 765 && color <= 1020) {	// синий макс, падает зелёный
		_r = 0;
		_g = 1020 - color;
		_b = 255;
	}
	else if (color > 1020 && color <= 1275) { 	// синий макс, растёт красный
		_r = color - 1020;
		_g = 0;
		_b = 255;
	}
	else if (color > 1275 && color <= 1530) {	// красный макс, падает синий
		_r = 255;
		_g = 0;
		_b = 1530 - color;
	}

    ledcWrite(3, _b); //briB
    ledcWrite(1, _r); //briR
    ledcWrite(2, _g); //briG
}

void LightCtrlTask( void * parameter )
{
    Serial.println("Light");
    ledcSetup(1, freq, resolution);
    ledcSetup(2, freq, resolution);
    ledcSetup(3, freq, resolution);
    ledcSetup(4, freq, resolution);

    ledcAttachPin(R, 1);
    ledcAttachPin(G, 2);
    ledcAttachPin(B, 3);
    ledcAttachPin(W, 4);

    int BRT_now = 0;
    int RGB_now = 0;
    while(1){
        if(LightCtrl){
            BRT_now = map(BRT_max,0,100,0,255);
            ledcWrite(4, BRT_now);}
        else{
            ledcWrite(4, 0);}

        if(RGBCtrl){
            RGB_now = map(RGB_set,0,100,0,1530);
            setRGB(RGB_now);
        }
        else{
            ledcWrite(3, 0); //briB
            ledcWrite(1, 0); //briR
            ledcWrite(2, 0); //briG
        }

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
        preferences.putBool("LightCtrl",LOW);
        preferences.putBool("FanCtrl",HIGH);
        preferences.putUInt("RGB_set",0);
        preferences.putBool("RGBCtrl",LOW);
        ts = true;
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
    RGB_set = preferences.getUInt("RGB_set");
    RGBCtrl = preferences.getBool("RGBCtrl");

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
    }}
    vTaskDelay(1000/portTICK_PERIOD_MS);
    }

    Serial.println("Ending TempRead");
    vTaskDelete( NULL );
}

void CompCtrlTask( void * parameter)
{
    Serial.println("CompCtrl");
    pinMode(Comp ,OUTPUT);
    digitalWrite(Comp,LOW);
    while(temp_cache<5||temp_cache>64){
    vTaskDelay(1000/portTICK_PERIOD_MS);
    }

    while(1){
    digitalWrite(Comp,Hysteresis(temp_cache));
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
    pinMode(F1 ,OUTPUT);
    pinMode(F2 ,OUTPUT);
    digitalWrite(F1,LOW);
    digitalWrite(F2,LOW);
    while(1){
    vTaskDelay(1000/portTICK_PERIOD_MS);        
    }

    Serial.println("Ending FanCtrl");
    vTaskDelete( NULL );
}

void RtcTask( void * parameter)
{
    Serial.println("RTC");
    xSemaphoreTake(i2c_mutex, portMAX_DELAY);
    Rtc.Begin();
    if (!Rtc.GetIsRunning()){
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);}
    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
    xSemaphoreGive(i2c_mutex);

    while(1){
    xSemaphoreTake(i2c_mutex, portMAX_DELAY);

    if(!Rtc.IsDateTimeValid()){
    Serial.println("RTC ERR!");
    if(ts){
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    Rtc.SetDateTime(compiled);
    ts=false;}
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