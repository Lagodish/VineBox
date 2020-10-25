#include <Arduino.h>
#include <Wire.h>
#include <menu.h>
#include <menuIO/u8g2Out.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include <GyverButton.h>
#include <config.h>
#include <RtcDS3231.h>

RtcDS3231<TwoWire> Rtc(Wire);
Preferences preferences;

GButton butt1(GP1); //GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);
GButton butt2(GP2);
GButton butt3(GP3);
GButton butt4(GP4);

using namespace Menu;

#define MAX_DEPTH 3

#define fontName u8g2_font_7x13_t_cyrillic //u8g2_font_7x13_mf
#define fontX 7
#define fontY 16
#define offsetX 0
#define offsetY 0
#define U8_Width 128
#define U8_Height 64
#define USE_HWI2C

U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0,22,21,U8X8_PIN_NONE);
typedef u8g2_uint_t u8g_uint_t;

bool oldRetCode; 
bool Hysteresis(double temp_now) {

  if(!Temp_mode){temp_now = int((temp_now*9/5)+32);}    

  bool retCode = false;
  if (temp_now > setted_temp+1.0) {
    retCode = true;
  } else if ( temp_now < setted_temp-PERF) {
    retCode = false;
  } else {
    retCode = oldRetCode;
  }
   
  oldRetCode = retCode;
  return retCode;
}

void beep(){
//TODO!
}

void writeTemp(){
  preferences.begin("VineBoxData", false);
  preferences.putDouble("setted_temp",setted_temp);
  preferences.end();
}

const colorDef<uint8_t> colors[6] MEMMODE={
  {{0,0},{0,1,1}},//bgColor
  {{1,1},{1,0,0}},//fgColor
  {{1,1},{1,0,0}},//valColor
  {{1,1},{1,0,0}},//unitColor
  {{0,1},{0,0,1}},//cursorColor
  {{1,1},{1,0,0}},//titleColor
};

result action6(eventMask e,navNode& nav, prompt &item) {

  if(Wireless==0){    //No

  }
  if(Wireless==1){    //WiFi

  }
 
  return proceed;
}

result action5(eventMask e,navNode& nav, prompt &item) {
    char userTime[8];
    userTime[0] = h_rtc / 10 + '0';
    userTime[1] = h_rtc % 10 + '0';
    userTime[2] = ':';
    userTime[3] = min_rtc / 10 + '0';
    userTime[4] = min_rtc % 10 + '0';
    userTime[5] = ':';
    userTime[6] = '0';
    userTime[7] = '0';
  
    RtcDateTime manual = RtcDateTime(__DATE__, userTime);
    Rtc.SetDateTime(manual);
    return proceed;
}

result action4(eventMask e,navNode& nav, prompt &item) {
  preferences.begin("VineBoxData", false);
  preferences.putUInt("PERF",PERF);
  preferences.end();
  return proceed;
}

result action3(eventMask e,navNode& nav, prompt &item) {
  preferences.begin("VineBoxData", false);
  preferences.putUInt("BRT_Disp",BRT_Disp);
  preferences.end();
  return proceed;
}

result action2(eventMask e,navNode& nav, prompt &item) {
  if(Temp_mode){ // C
    setted_temp = int((setted_temp-32)*5/9);
  }
  else{ //  F
    setted_temp = int((setted_temp*9/5)+32);
  }

  preferences.begin("VineBoxData", false);
  preferences.putDouble("setted_temp",setted_temp);
  preferences.putBool("Temp_mode",Temp_mode);
  preferences.end();

  return proceed;
}

result action1(eventMask e,navNode& nav, prompt &item) {  
  preferences.begin("VineBoxData", false);
  preferences.putUInt("BRT_max",BRT_max);
  preferences.putUInt("SPD_max",SPD_max);
  preferences.putBool("LightCtrl",LightCtrl);
  preferences.putBool("FanCtrl",FanCtrl);
  preferences.putUInt("Wireless",Wireless);
  preferences.end();

  return proceed;
}


TOGGLE(LightCtrl,setLight,text_2,action1,enterEvent,noStyle
  ,VALUE(text_8,HIGH,doNothing,noEvent)
  ,VALUE(text_9,LOW,doNothing,noEvent)
);


TOGGLE(FanCtrl,setFan,text_3,action1,enterEvent,noStyle
  ,VALUE(text_8,HIGH,doNothing,noEvent)
  ,VALUE(text_9,LOW,doNothing,noEvent)
);


TOGGLE(Temp_mode,TempMenu,text_4,action2,enterEvent,noStyle
  ,VALUE(text_22,HIGH,doNothing,noEvent)  //C
  ,VALUE(text_23,LOW,doNothing,noEvent)   //F
);

TOGGLE(Wireless,setWireless,text_6,action1,enterEvent,noStyle
  ,VALUE(text_9,0,doNothing,noEvent)
  ,VALUE("WiFi",1,doNothing,noEvent)
);


TOGGLE(PERF,PerformanceMenu,text_7,action4,enterEvent,noStyle
  ,VALUE(text_19,3,doNothing,noEvent)
  ,VALUE(text_20,2,doNothing,noEvent)
  ,VALUE(text_21,1,doNothing,noEvent)
);

MENU(LightMenu,text_16,doNothing,noEvent,noStyle
  ,SUBMENU(setLight)
  ,FIELD(BRT_max,text_18,"%",0,100,10,0,action1,enterEvent,wrapStyle)
  ,EXIT(text_11)
);

MENU(FanMenu,text_15,doNothing,noEvent,noStyle
  ,SUBMENU(setFan)
  ,FIELD(SPD_max,text_17,"%",0,100,10,0,action1,enterEvent,wrapStyle)
  ,EXIT(text_11)
);

PADMENU(HM_Menu,text_13,doNothing,noEvent,noStyle
  ,FIELD(h_rtc,"",":",0,23,1,0,action5,enterEvent,wrapStyle)
  ,FIELD(min_rtc,"","",0,59,1,0,action5,enterEvent,wrapStyle)
);

MENU(timeMenu,text_12,doNothing,noEvent,noStyle
  ,SUBMENU(HM_Menu)
  ,EXIT(text_11)
);


//TODO работа на нагрев (если в комнате температура меньше чем нужно) + счетчик наработки
MENU(mainMenu, text_1 ,doNothing,noEvent,noStyle
  ,SUBMENU(timeMenu)
  ,SUBMENU(LightMenu)
  ,SUBMENU(FanMenu)
  ,SUBMENU(PerformanceMenu)
  ,SUBMENU(setWireless)
  ,SUBMENU(TempMenu)
  ,FIELD(BRT_Disp,text_10," %",0,100,10,0,action3,enterEvent,noStyle)
  ,EXIT(text_11)
);

//serialIn serial(Serial1);
//MENU_INPUTS(in, NONE);  //&serial
chainStream<0> in(NULL);//<-- this creates a NULL stream

MENU_OUTPUTS(out,MAX_DEPTH
  ,U8G2_OUT(u8g2,colors,fontX,fontY,offsetX,offsetY,{0,0,U8_Width/fontX,U8_Height/fontY})
  ,NONE//SERIAL_OUT(Serial1)
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

//when menu is suspended
result MainScreen(menuOut& o,idleEvent e) {
  o.clear();
  if(showTemp){tempPrint = setted_temp;}
  else{
    if(Temp_mode){tempPrint = temp_cache;}
    else{tempPrint = int((temp_cache*9/5)+32);}}
  
  switch(e) {
    case idleStart:/*o.println("suspending menu!")*/;break;
    case idling:{
    mainScreenOn=true;
    if(tempPrint<9.95){
    u8g2.setFont(u8g2_font_ncenB24_tf);
    u8g2.drawUTF8(83, 33, DEGREE_SYMBOL);
    u8g2.setFont(u8g2_font_fur35_tr);
    o.setCursor(2,2); 
    o.print(String(tempPrint,1)); 
    u8g2.setFont(u8g2_font_fur30_tr);
    o.setCursor(13,2);
    }
    else{
    u8g2.setFont(u8g2_font_ncenB24_tf);
    u8g2.drawUTF8(91, 33, DEGREE_SYMBOL);
    u8g2.setFont(u8g2_font_fur35_tr);
    o.setCursor(0,2);
    o.print(String(tempPrint,1)); 
    u8g2.setFont(u8g2_font_fur30_tr);
    o.setCursor(14,2);
    }
    if(Temp_mode){o.print("C");}else{o.print("F");}
    u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t); //{Eeeeeeeeeeeeeeeee}
    if(timer_1>0){u8g2.drawUTF8(8, 64, setup_SYMBOL);}
    if(Wireless==1){u8g2.drawUTF8(72, 64, wifi_SYMBOL);}
    //u8g2.drawUTF8(72, 64, SYMBOL);
    if(err_flag){u8g2.drawUTF8(106, 64, ALERT_SYMBOL);}
    break;}
    case idleEnd:/*o.println("resuming menu.");*/mainScreenOn=false;u8g2.setFont(fontName);break;
  }

  return proceed;
}
