#include <DallasTemperature.h>
#include <OneWire.h>
#include<EEPROM.h>

#define ONE_WIRE_BUS 4
#define Pressure_sensor A0
#define Load D0
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
int Hr = 0,Min = 0,Sec = 0,Day = 0,oldtime;
int Hr_Addr = 00,min_Addr = 01,day_Addr = 02;
int pinValue,notification = 0;
OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "mN4XOe1f8Ehlkfj3oyf8lUSxvOVoLFms";
char ssid[] = "iotesp";
char pass[] = "123456789";

BLYNK_READ(V1) // Widget in the app READs Virtal Pin V5 with the certain frequency
{
// This command writes Arduino's uptime in seconds to Virtual Pin V5
Blynk.virtualWrite(1, 10);
}

BLYNK_WRITE(V5) // V5 is the number of Virtual Pin
{
pinValue = param.asInt();
}

void setup()
{
Serial.begin(9600);
sensors.begin();
Blynk.begin(auth, ssid, pass);
//  EEPROM.write(Hr_Addr,0);
//  EEPROM.write(min_Addr,0);
//  EEPROM.write(day_Addr,0);
Day = EEPROM.read(day_Addr);
Hr = EEPROM.read(Hr_Addr);
Min = EEPROM.read(min_Addr);
pinMode(D0,OUTPUT);
digitalWrite(D0,LOW);
delay(1000);
}

void loop()
{
Blynk.run();
Serial.print("button ");
Serial.println(pinValue);
sensors.requestTemperatures();                // Send the command to get temperatures
Serial.println("Temperature is: ");
float val1 = sensors.getTempCByIndex(0);
Serial.println(val1);   // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
Blynk.virtualWrite(1, val1);
if(Min > 3)// Service notification
{
Blynk.virtualWrite(6, "Service date Exp");
}
else
{
Blynk.virtualWrite(6, "----");
}
if(val1 > 50)
{
Blynk.virtualWrite(0, "OVER TEMPERATURE");
digitalWrite(Load,LOW);
}
else
{
Blynk.virtualWrite(0, "NORMAL");
if(pinValue == 1)
{
digitalWrite(Load,HIGH);
}
else
{
digitalWrite(Load,LOW);
}
//      Blynk.run();
}
float val = analogRead(Pressure_sensor);
val = (val *175)/1024;
Serial.print("Pressure: ");
Serial.println(val);
Blynk.virtualWrite(2, val);
Serial.println(pinValue);
if(pinValue == 1)
{
if((millis() - oldtime) >900)
{
Sec ++;
oldtime = millis();
Serial.println(Sec);
}
if(Hr >= 24)
{
Day = EEPROM.read(day_Addr);
Day++;
EEPROM.write(day_Addr,Day);
Hr = 0;
}
if(Min >= 60)
{
Hr = EEPROM.read(Hr_Addr);
Hr++;
EEPROM.write(Hr_Addr , Hr);
Min = 0;
}
if(Sec >= 60)
{
//      Min = EEPROM.read(min_Addr);
Min++;
EEPROM.write(min_Addr,Min);
Sec = 0;
}
String buff = "Days:"+String(Day)+"  "+String(Hr)+":"+String(Min);
Serial.println(buff);
Blynk.virtualWrite(3, buff);

}
else
{
oldtime = millis();
}
}
