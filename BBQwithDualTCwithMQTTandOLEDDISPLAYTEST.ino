#include <Adafruit_MAX31856.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
 
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
 
// Configure Wifi with your ssid and password
const char* ssid     = "seemusic";
const char* password = "12string";

//Configure MQTT
#define AIO_SERVER      "192.168.33.195"
#define AIO_SERVERPORT  1883  
#define AIO_USERNAME    " "
#define AIO_KEY         "...your AIO key..."

// Configure Sensors

// Thermocouple 0
#define tc0name   "TC0: "
#define tc0sck    17
#define tc0sdo    5
#define tc0sdi    18
#define tc0cs     23
#define tc0offset -3         // Sensor calibration offset, in C

// Thermocouple 1
#define tc1name   "TC1: "
#define tc1sck    13
#define tc1sdo    12
#define tc1sdi    14
#define tc1cs     27
#define tc1offset -3       // Sensor calibration offset, in C

// Setup for Heltec OLED
// Full Buffer
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);
// Page Buffer
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);
// U8x8
//U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);


 
WiFiClient network;

// Initialize MQTT client & pubs
//Adafruit_MQTT_Client mqtt(&network, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
//Adafruit_MQTT_Publish tc0temptt = Adafruit_MQTT_Publish(&mqtt, "/tc0temp");
//Adafruit_MQTT_Publish tc1temptt = Adafruit_MQTT_Publish(&mqtt, "/tc1temp");
 
 
// Initialize MAX31865 amp with software SPI: CS, DI, DO, CLK
Adafruit_MAX31856 maxthermo0 = Adafruit_MAX31856(tc0cs, tc0sdi, tc0sdo, tc0sck);
Adafruit_MAX31856 maxthermo1 = Adafruit_MAX31856(tc1cs, tc1sdi, tc1sdo, tc1sck);
 
// use hardware SPI, just pass in the CS pin
//Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(10);
 
void setup() {
  Serial.begin(115200);
  
  // Setup screen
  u8g2.begin();
 
  // Setup Wifi
  Serial.print("Connecting to ");
    Serial.println(ssid);
 
    WiFi.begin(ssid, password);
 
    //while (WiFi.status() != WL_CONNECTED) {
    //    delay(500);
    //    Serial.print(".");
    //}
 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
      
  maxthermo0.begin();
 
  maxthermo0.setThermocoupleType(MAX31856_TCTYPE_K);
 
  Serial.print("TC0 type: ");
  switch (maxthermo0.getThermocoupleType() ) {
    case MAX31856_TCTYPE_B: Serial.println("B Type"); break;
    case MAX31856_TCTYPE_E: Serial.println("E Type"); break;
    case MAX31856_TCTYPE_J: Serial.println("J Type"); break;
    case MAX31856_TCTYPE_K: Serial.println("K Type"); break;
    case MAX31856_TCTYPE_N: Serial.println("N Type"); break;
    case MAX31856_TCTYPE_R: Serial.println("R Type"); break;
    case MAX31856_TCTYPE_S: Serial.println("S Type"); break;
    case MAX31856_TCTYPE_T: Serial.println("T Type"); break;
    case MAX31856_VMODE_G8: Serial.println("Voltage x8 Gain mode"); break;
    case MAX31856_VMODE_G32: Serial.println("Voltage x8 Gain mode"); break;
    default: Serial.println("Unknown"); break;
  }

  maxthermo1.begin();
 
  maxthermo1.setThermocoupleType(MAX31856_TCTYPE_K);
 
  Serial.print("TC1 type: ");
  switch (maxthermo1.getThermocoupleType() ) {
    case MAX31856_TCTYPE_B: Serial.println("B Type"); break;
    case MAX31856_TCTYPE_E: Serial.println("E Type"); break;
    case MAX31856_TCTYPE_J: Serial.println("J Type"); break;
    case MAX31856_TCTYPE_K: Serial.println("K Type"); break;
    case MAX31856_TCTYPE_N: Serial.println("N Type"); break;
    case MAX31856_TCTYPE_R: Serial.println("R Type"); break;
    case MAX31856_TCTYPE_S: Serial.println("S Type"); break;
    case MAX31856_TCTYPE_T: Serial.println("T Type"); break;
    case MAX31856_VMODE_G8: Serial.println("Voltage x8 Gain mode"); break;
    case MAX31856_VMODE_G32: Serial.println("Voltage x8 Gain mode"); break;
    default: Serial.println("Unknown"); break;
  }
 
 
}
 
void loop() {
 
  float tc0temp = 0, tc0tempf = 0, tc1temp = 0, tc1tempf = 0;
  char displ0name[6] = tc0name;
  char displ1name[6] = tc1name;
  char displtemp0[5];
  char displtemp1[5];
  char* displ0;
  char* displ1;
  
 
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_logisoso18_tf); // choose a suitable font
 
  //Serial.print("CJ0 Temp: ");
  //Serial.println(maxthermo0.readCJTemperature());
 
  Serial.print(displ0name);
  tc0temp = maxthermo0.readThermocoupleTemperature() + tc0offset;
  tc0tempf = (1.8 * tc0temp) + 32;
  dtostrf(tc0tempf, 4, 0, displtemp0);
  displ0[0] = '\0';
  strcat( displ0, displ0name);
  strcat( displ0, displtemp0);
  Serial.println(displtemp0);
  u8g2.drawStr(0,30,displ0);
 
  // Check and print any faults
  uint8_t fault0 = maxthermo0.readFault();
  if (fault0) {
    if (fault0 & MAX31856_FAULT_CJRANGE) Serial.println("Cold Junction Range Fault");
    if (fault0 & MAX31856_FAULT_TCRANGE) Serial.println("Thermocouple Range Fault");
    if (fault0 & MAX31856_FAULT_CJHIGH)  Serial.println("Cold Junction High Fault");
    if (fault0 & MAX31856_FAULT_CJLOW)   Serial.println("Cold Junction Low Fault");
    if (fault0 & MAX31856_FAULT_TCHIGH)  Serial.println("Thermocouple High Fault");
    if (fault0 & MAX31856_FAULT_TCLOW)   Serial.println("Thermocouple Low Fault");
    if (fault0 & MAX31856_FAULT_OVUV)    Serial.println("Over/Under Voltage Fault");
    if (fault0 & MAX31856_FAULT_OPEN)    Serial.println("Thermocouple Open Fault");
  }
  //MQTT_connect();
  //tc0temptt.publish(tc0tempf);
  
  //Serial.print("CJ1 Temp: ");
  //Serial.println(maxthermo1.readCJTemperature());
 
  Serial.print(displtemp1);
  tc1temp = maxthermo1.readThermocoupleTemperature() + tc1offset;
  tc1tempf = (1.8 * tc1temp) + 32;
  dtostrf(tc1tempf, 4, 0, displtemp1);
  displ1 = strcat(displ1name, displtemp1);
  Serial.println(displtemp1);
  u8g2.drawStr(0,60,displ1);
  
  // Check and print any faults
  uint8_t fault1 = maxthermo1.readFault();
  if (fault1) {
    if (fault1 & MAX31856_FAULT_CJRANGE) Serial.println("Cold Junction Range Fault");
    if (fault1 & MAX31856_FAULT_TCRANGE) Serial.println("Thermocouple Range Fault");
    if (fault1 & MAX31856_FAULT_CJHIGH)  Serial.println("Cold Junction High Fault");
    if (fault1 & MAX31856_FAULT_CJLOW)   Serial.println("Cold Junction Low Fault");
    if (fault1 & MAX31856_FAULT_TCHIGH)  Serial.println("Thermocouple High Fault");
    if (fault1 & MAX31856_FAULT_TCLOW)   Serial.println("Thermocouple Low Fault");
    if (fault1 & MAX31856_FAULT_OVUV)    Serial.println("Over/Under Voltage Fault");
    if (fault1 & MAX31856_FAULT_OPEN)    Serial.println("Thermocouple Open Fault");
  }
  u8g2.sendBuffer();          // transfer internal memory to the display
  //MQTT_connect();
  //tc1temptt.publish(tc1tempf);
  delay(1000);
}
 
// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;
 
  // Stop if already connected.
//  if (mqtt.connected()) {
//    return;
//  }
 
  Serial.print("Connecting to MQTT... ");
 
  uint8_t retries = 3;
//  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
//       Serial.println(mqtt.connectErrorString(ret));
//       Serial.println("Retrying MQTT connection in 5 seconds...");
//       mqtt.disconnect();
//       delay(5000);  // wait 5 seconds
//       retries--;
//       if (retries == 0) {
//         // basically die and wait for WDT to reset me
//         while (1);
//       }
  }
//  Serial.println("MQTT Connected!");
//}
