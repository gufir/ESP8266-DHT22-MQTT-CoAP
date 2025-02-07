#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define DHTPIN 2          // Pin D4 ESP
#define DHTTYPE DHT22      // DHT22 Sensor

const char* mqtt_server = "192.168.43.192";
const char* mqtt_username = "pi";
const char* mqtt_password = "raspberry";
const char* mqtt_topic = "sensorDHT22";

const char* ssid = "ASDE";
const char* password = "1234566678";

WiFiClient espClient;
PubSubClient client(espClient);
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

void BrokerConnect(void *parameter) {
    client.setServer(mqtt_server, 1883);
    while (!client.connected()) {
        Serial.println("Connecting to MQTT broker...");
        if (client.connect("ESP8266Client", mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT broker");
        } else {
            Serial.print("Failed with state ");
            Serial.println(client.state());
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
    vTaskDelete(NULL);
}

void SensorTask(void *parameter) {
    while (1) {
        float temperature = dht.readTemperature();
        float humidity = dht.readHumidity();

        DynamicJsonDocument doc(1024);
        doc["Sensor Node"] = "Sensor Node 1";
        doc["Protocol"] = "MQTT";
        doc["Temperature"] = temperature;
        doc["Humidity"] = humidity;
        
        String jsonString;
        serializeJson(doc, jsonString);
        Serial.println(jsonString);
        client.publish(mqtt_topic, jsonString.c_str());
        
        vTaskDelay(300000 / portTICK_PERIOD_MS); // Delay 5 menit
    }
}

void setup() {
    Serial.begin(115200);
    dht.begin();
    
    xTaskCreate(WiFiSetup, "WiFiSetup", 4096, NULL, 1, NULL);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    
    xTaskCreate(BrokerConnect, "BrokerConnect", 4096, NULL, 1, NULL);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    
    xTaskCreate(SensorTask, "SensorTask", 4096, NULL, 1, NULL);
}

void loop() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
