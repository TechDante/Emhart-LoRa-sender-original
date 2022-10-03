#include <heltec.h>
#include <LoRa.h>
#include <SPI.h>
#include <Arduino.h>
#include "DHT.h"
#include "DHT.cpp"
#include "esp_system.h"

// Image includes
#include "images/link.xbm"

// #define OPTIMIZE

#define IMG_MARGIN 2

#define DHT_SENSOR_PIN 21
#define DHT_SENSOR_TYPE DHT11

#define BAND 433920000
#define PABOOST false
#define TXPOWER 14
#define SPREADING_FACTOR 7
#define BANDWIDTH 500000
#define CODING_RATE 5
#define PREAMBLE_LENGTH 6
#define SYNC_WORD 0x34

#define SS 18
#define RST 14
#define DI0 26

DHT dht(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);
String id = "";
int counter;

String getMacAddress() {
  uint8_t baseMac[6];
  // Get MAC address for WiFi station
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  char baseMacChr[18] = {0};
  sprintf(baseMacChr, "%02x%02x%02x%02x%02x%02x", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  return String(baseMacChr);
}

void setup() {
  id = getMacAddress();
  
  // Serial setup
  #ifndef OPTIMIZE
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Station: \"" + id + "\"");

  // Display setup
  Heltec.begin(/*Enable display*/true, /*Enable LoRa*/true, /*Enable serial*/true, /*PA boost*/true, BAND);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->clear();
  #else
  Heltec.begin(/*Enable display*/false, /*Enable LoRa*/true, /*Enable serial*/false, /*PA boost*/true, BAND);
  #endif

  // LoRa setup
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS,RST,DI0);
  LoRa.setFrequency(BAND);
  LoRa.setSyncWord(SYNC_WORD);
  LoRa.setSpreadingFactor(SPREADING_FACTOR);
  LoRa.setCodingRate4(CODING_RATE);
  LoRa.setSignalBandwidth(BANDWIDTH);
  LoRa.setPreambleLength(PREAMBLE_LENGTH);
  LoRa.enableCrc(); 
  

  // DHT setup
  dht.begin();
}

void loop() {
  #ifndef OPTIMIZE
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "ID: " + id);
  #endif

  float temp = dht.readTemperature();
  float humi = dht.readHumidity();
  Serial.println(temp);
  Serial.println(humi);

  /* digitalWrite(DHT_SENSOR_PIN, LOW);
  delay(5000);
  digitalWrite(DHT_SENSOR_PIN, HIGH);
  delay(5000);
*/
 


  #ifndef OPTIMIZE
  if ((1+counter)%2)
    Heltec.display->drawXbm(128-link_width-IMG_MARGIN, IMG_MARGIN, link_width, link_height, link_bits);

  Heltec.display->drawString(0, 15, "Temperature: " + String(temp));
  Heltec.display->drawString(0, 25, "Humidity: " + String(humi));
  Heltec.display->display();
  #endif
  
  // Construct packet
  String json = "{\"id\":\"" + id + "\",\"t\":\"" + temp + "\",\"h\":\"" + humi + "\"}";

  // Send packet
  LoRa.beginPacket();
  LoRa.print(json);
  LoRa.endPacket();

  counter++;

  delay(1000);
}