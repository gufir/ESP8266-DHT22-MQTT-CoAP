#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <coap-simple.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define DHTPIN 2
#define DHTTYPE DHT22

const char* ssid = "ASDE";
const char* password = "1234566678";

WiFiUDP udp;
Coap coap(udp);
DHT dht(DHTPIN, DHTTYPE);

void WiFiSetup(void *parameter) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    vTaskDelete(NULL);
}

void CoAPTask(void *parameter) {
    while (1) {
        float temperature = dht.readTemperature();
        float humidity = dht.readHumidity();

        DynamicJsonDocument doc(1024);
        doc["Sensor"] = "DHT22";
        doc["Protocol"] = "CoAP";
        doc["Humidity"] = humidity;
        doc["Temperature"] = temperature;

        String jsonString;
        serializeJson(doc, jsonString);
        Serial.println(jsonString);

        coap.post(IPAddress(192, 168, 43, 192), 5683, "SensorData", jsonString.c_str());
        
        vTaskDelay(300000 / portTICK_PERIOD_MS); // Delay 5 menit
    }
}

void setup() {
    Serial.begin(115200);
    dht.begin();
    coap.start();
    
    xTaskCreate(WiFiSetup, "WiFiSetup", 4096, NULL, 1, NULL);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    
    xTaskCreate(CoAPTask, "CoAPTask", 4096, NULL, 1, NULL);
}

void loop() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
