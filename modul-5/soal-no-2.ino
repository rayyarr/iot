// Node 1 (Pertama)

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

WiFiClient espClient;
PubSubClient client(espClient);

#define WIFI_SSID "Karyawan"
#define WIFI_PASS "@Gaspol2023ok"
#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883
const char *topic = "kelompok-nanas/sensor/temp";

#define DHTPIN D4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define LED1 D3
bool ledStatus = true;

unsigned long _waiting = millis();
unsigned long _now;
int value = 0;
char data[50];
char data2[50];

String buffData;

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//void callback(char* topic, byte* payload, unsigned int length){}

void callback(char* topic, byte* payload, unsigned int length) {
  buffData = "";
  for (int i = 0; i < length; i++) {
    buffData += (char)payload[i];
  }

  Serial.print(topic);
  Serial.print(" ==> ");
  Serial.println(buffData);

  if (buffData == "false") {
    digitalWrite(LED1, LOW);
  } else {
    digitalWrite(LED1, HIGH);
  }
  /*if (strcmp(buffData.c_str(), "true") == 0) {
    digitalWrite(LED1, LOW);
  } else if (strcmp(buffData.c_str(), "false") == 0) {
    digitalWrite(LED1, HIGH);
  }*/
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan MQTT... ");
    String clientId = "ESP8266-Ray-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Berhasil");
      client.publish(topic, "Yess... saya terkoneksi");
      client.subscribe("kelompok-nanas/act/led");
      kirimDetik();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  dht.begin();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  kirimDetik();

}

void kirimDetik() {
  _now = millis();
  if (millis() - _waiting > 1000) {
    _waiting = _now;
    value++;
    
    float suhu = dht.readTemperature();

    if (!isnan(suhu)) {
      Serial.print("Suhu: ");
      Serial.println(suhu);
      
      char buffer[10]; // Buffer untuk mengonversi float menjadi string
      dtostrf(suhu, 4, 2, buffer); // Mengonversi float menjadi string dengan 2 desimal
      client.publish(topic, buffer); // Kirim suhu ke topik MQTT
    }
  }
}









// Node 2 (Kedua)

#include <WiFi.h> // ESP32
#include <PubSubClient.h>
#include <Arduino.h>

WiFiClient espClient;
PubSubClient client(espClient);
String buffData = "";

#define WIFI_SSID "Karyawan"
#define WIFI_PASS "@Gaspol2023ok"
#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883
const char *topicPub = "kelompok-nanas/act/led";

#define LED1 2
bool ledStatus = true;

unsigned long _waiting = millis();
unsigned long _now;
int value = 0;
char data[50];
char data2[50];

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topicPub, byte* payload, unsigned int length) {
  buffData = "";
  for (int i = 0; i < length; i++) {
    buffData += (char)payload[i];
  }

  Serial.print(topicPub);
  Serial.print(" ==> ");
  Serial.println(buffData);

  if (buffData.toFloat() > 29.0) {
    //Serial.print("true");
    ledStatus = true;
    kirimDetik();
  } else {
    //Serial.print("false");
    ledStatus = false;
    kirimDetik();
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan MQTT... ");
    String clientId = "ESP8266-Ray-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Berhasil");
      client.publish(topicPub, "Yess... saya terkoneksi");
      client.subscribe("kelompok-nanas/sensor/temp");
      kirimDetik();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  
  pinMode(LED1, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  kirimDetik();
}

void kirimDetik() {
  _now = millis();
  if (millis() - _waiting > 1000) {
    _waiting = _now;
    sprintf(data, "true");
    sprintf(data2, "false");
    if (ledStatus) {
        client.publish(topicPub, data);
    } else {
        client.publish(topicPub, data2);
    }
    
  }
}
