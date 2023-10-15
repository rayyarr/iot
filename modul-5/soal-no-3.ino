#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

WiFiClient espClient;
PubSubClient client(espClient);

#define WIFI_SSID "Ana"
#define WIFI_PASS "azahra2023"
#define MQTT_SERVER "192.168.1.8"
#define MQTT_PORT 1883
const char *topic = "ray";

#define DHTPIN D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define LED1 D2
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

void callback(char* topic, byte* payload, unsigned int length){}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan MQTT... ");
    String clientId = "ESP8266-Ray-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Berhasil");
      client.publish(topic, "Yess... saya terkoneksi");
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
  if (millis() - _waiting > 5000) {
    _waiting = _now;
    value++;
    float suhu = dht.readTemperature();
    sprintf(data, "LED Menyala - Suhu %.2f", suhu);
    sprintf(data2, "LED Mati - Suhu %.2f", suhu);
    if (suhu > 29){
      digitalWrite(LED1, HIGH);
      ledStatus = true;
    } else {
      digitalWrite(LED1, LOW);
      ledStatus = false;
    }
    if (ledStatus) {
      client.publish(topic, data);
    } else {
      client.publish(topic, data2);
    }
  }
}
