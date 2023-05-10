
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// PIN DHT22
#define DHTPIN 2          // Pin D4 ESP
#define DHTTYPE DHT22      // DHT22 Sensor

// MQTT Broker and User 
const char* mqtt_server = "192.168.43.192";
const char* mqtt_username = "pi";
const char* mqtt_password = "raspberry";
const char* mqtt_topic = "sensorDHT22";

// Koneksi WiFi 
const char* ssid = "ASDE";
const char* password = "1234566678";

WiFiClient espClient;
PubSubClient client(espClient);

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

void BrokerConnect(){

  // Connect to MQTT broker
  client.setServer(mqtt_server,1883);
  
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("ESP8266Client", mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  
}


void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFiSetup();
  BrokerConnect();

}

void loop() {
  int interval = 300000;
  const long currentmillis = millis();

  if(currentmillis<=interval){

    // Read Data Sensor DHT22
    delay(2000); //Sensor Response Need Delay 2 s
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Convert to JSON
    DynamicJsonDocument doc(1024);
    doc["Sensor Node"] = "Sensor Node 1";
    doc["Protocol"] = "MQTT";
    doc["Temperature"] = temperature;
    doc["Humidity"] = humidity;
    String jsonString;
    serializeJson(doc, jsonString);
    Serial.println(jsonString);

    //Publish to MQTT
    client.publish(mqtt_topic,jsonString.c_str());
  }
  else{
    // Disconnecting from Broker
    client.disconnect();
    Serial.println("Client Disconnected");
    
  }

}
