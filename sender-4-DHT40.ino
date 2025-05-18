//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Libraries for temperature sensor
#include "Adafruit_SHT4x.h"




//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 23
#define DI0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
//Need to be aligned with the frequency of the OpenMQTTGateway
#define BAND 868E6

//Bandwidth (7.8 / 10.4 / 15.6 / 20.8 / 31.25 / 41.7 / 62.5 / 125 / 250 / 500), default 125K
#define BANDWIDTH 125E3
//SpreadingFactor from 6-12,default 7 see API docs
#define SPREADINGFACTOR 10

//OLED pins
#define OLED_SDA 21
#define OLED_SCL 22
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//Set Battery pin
#define vbatPin 35
#define battery_max 4.2
#define battery_min 3.4

//Verbose button pin
#define buttonPin 0
    //if pin0 is shorted to GND while startup, the screen will be keep on
    //debug mode can also be enable by sending any text through serial at anytime

#define SLEEP_TIMER 10*60*1000000 //10min
//#define SLEEP_TIMER 10*1000000 //10s



//setup Sensor
Adafruit_SHT4x sht4 = Adafruit_SHT4x();
//setup screen
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

float VBAT;
float BAT;
int DebugState = 1;





void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  Serial.println("LoRa Sender Test");

  //initialize pin mode for battery reading
  pinMode(vbatPin, INPUT);

  //initialize pin for debug button
  pinMode(buttonPin, INPUT);
  DebugState = digitalRead(buttonPin);
  
  //initialize temperature sensor
  sht4.begin();
  if (!sht4.begin()) {
    Serial.println("Couldn't find SHT4x");
    while (1) delay(10);
  }
  Serial.println("Found SHT4x sensor");

  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSpreadingFactor(SPREADINGFACTOR);
  LoRa.setSignalBandwidth(BANDWIDTH);
  
  Serial.println("LoRa Initializing OK!");

  //Clean id for display
  String ID = Network.macAddress();
  ID.remove(0,12);
  ID.remove(2,1);
    
  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  //print startup text
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LoRa Initializing OK!");
  display.setCursor(0,10);
  display.println("ID :");
  display.setTextSize(3);
  display.setCursor(30,20);
  display.println(ID);
  display.display();
  delay(2000);

  //Get first values
  sensors_event_t humidity, temp;
  sht4.getEvent(&humidity, &temp);
  // Read the humidity in %:
  float h = humidity.relative_humidity;
  // Read the temperature as Celsius:
  float t = temp.temperature;

  delay(1000);
  // Read the battery
  VBAT = ((float)(analogRead(vbatPin)) / 4095)*2*3.3 + 0.18;
  BAT = ((VBAT - battery_min) / (battery_max - battery_min)) * 100;

  //Make message
  String msg = "{\"id\":\"" + ID + "\",\"model\":\"LORA32 TTGO\",\"temperature\":" + String(t) + ",\"humidity\":" + String(h) + ",\"battery_volt\":" + String(VBAT) + ",\"battery_percent\":" + String(BAT) + "}";
  //Send throught serial for debug
  Serial.println(msg);

  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(msg);
  LoRa.endPacket();

  
  //Print first values
  display.clearDisplay();
  //display.setCursor(X,Y); //coordonées en pixel, 4 par caractere + 1 d'espces
  display.setTextSize(0.8);
  display.setCursor(0,0);
  display.println("ID :");
  display.setCursor(90,0);
  display.println(ID);
  display.setCursor(0,20);
  display.setTextSize(1);
  display.print("");
  display.setCursor(0,30);
  display.print("Temperature:");
  display.setCursor(90,30);
  display.print(t);
  display.setCursor(0,40);
  display.print("Humidite:");
  display.setCursor(90,40);
  display.print(h);
  display.setCursor(0,50);
  display.print("Battery:");
  display.setCursor(90,50);
  display.print(BAT);
  display.display();
  delay(10000);

  //shutdown display
  display.clearDisplay();
  display.display();
}





void loop() {

  String ID = Network.macAddress();
  ID.remove(0,12);
  ID.remove(2,1);

  //wake up sensor
  sensors_event_t humidity, temp;
  sht4.getEvent(&humidity, &temp);
  // Read the humidity in %:
  float h = humidity.relative_humidity;
  // Read the temperature as Celsius:
  float t = temp.temperature;
  
  // Read the battery
  VBAT = ((float)(analogRead(vbatPin)) / 4095)*2*3.3 + 0.18;
  BAT = ((VBAT - battery_min) / (battery_max - battery_min)) * 100;

  // Check if any reads failed and exit early (to try again):
  if (isnan(h) || isnan(t)) {
    String msg = "Failed to read from SHT sensor!";
    Serial.println(msg);
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
    return;
  }
  String msg = "{\"id\":\"" + ID + "\",\"temperature\":" + String(t) + ",\"humidity\":" + String(h) + ",\"battery\":" + String(BAT) + "}";
   Serial.println(msg);

  //Send LoRa packet to receiver
  LoRa.setSpreadingFactor(SPREADINGFACTOR);
  LoRa.setSignalBandwidth(BANDWIDTH);
  LoRa.beginPacket();
  LoRa.print(msg);
  LoRa.endPacket();

  //If debug on (throught serial or pin) displays values on screen
  if (Serial.available() > 0 or DebugState == LOW) {
    display.clearDisplay();
    //display.setCursor(X,Y); //coordonées en pixel, 4 par caractere + 1 d'espces
    display.setTextSize(0.8);
    display.setCursor(0,0);
    display.println("ID :");
    display.setCursor(90,0);
    display.println(ID);
    display.setCursor(0,20);
    display.setTextSize(1);
    display.print("");
    display.setCursor(0,30);
    display.print("Temperature:");
    display.setCursor(90,30);
    display.print(t);
    display.setCursor(0,40);
    display.print("Humidite:");
    display.setCursor(90,40);
    display.print(h);
    display.setCursor(0,50);
    display.print("Battery:");
    display.setCursor(90,50);
    display.print(BAT);
    display.display();
  }

  esp_sleep_enable_timer_wakeup(SLEEP_TIMER);
  esp_light_sleep_start();
}
