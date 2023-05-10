#include <ESP8266WiFi.h>
#include <coap-simple.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>

#define DHTPIN 2
#define DHTTYPE DHT22

// WiFi SSID and Pass
const char* ssid = "ASDE";
const char* password = "1234566678";

//callback
void callback_response(CoapPacket &packet, IPAddress ip, int port);

WiFiUDP udp;
Coap coap(udp);

DHT dht(DHTPIN, DHTTYPE);

void WiFiSetup(){
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFiSetup();
  coap.start();
}

void loop() {

  int interval = 300000;
  const long currentmillis = millis();

  if(currentmillis<=interval){
    delay(2000);
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    //JSON Object
    DynamicJsonDocument doc(1024);
    doc["Sensor"] = "DHT22";
    doc["Protocol"] = "CoAP";
    doc["Humidity"] = humidity;
    doc["Temperature"] = temperature;
 
    String jsonString;
    serializeJson(doc, jsonString);
    Serial.println(jsonString);

    coap.post(IPAddress(192,168,43,192),5683,"SensorData",jsonString.c_str());
  }
}
