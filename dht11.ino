#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#define DHTPIN D1
#define LED D2

#define DHTTYPE DHT11 // DHT 11
// #define DHTTYPE DHT22  // DHT 22 (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "";
const char* password = "";
String serverName = "";
unsigned long lastTime = 0;
// unsigned long timerDelay = 3600000;
unsigned long timerDelay = 30000;

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  dht.begin();
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  pinMode(LED,OUTPUT);
}

void loop() {

  delay(2000);
  // float h = 12.12;
  // float t = 87.35;
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      String humi = String(h);
      String temp = String(t);

      String serverPath = serverName + "?humi=" + humi + "&temp=" + temp + "&deviceid=dhtroom4&room=4";
      http.begin(client, serverPath.c_str());
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        if(payload == "Completed"){
          Serial.println("LED ON");
          digitalWrite(LED,1);
          delay(5000);
          Serial.println("LED OFF");
          digitalWrite(LED,0);
        }
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
