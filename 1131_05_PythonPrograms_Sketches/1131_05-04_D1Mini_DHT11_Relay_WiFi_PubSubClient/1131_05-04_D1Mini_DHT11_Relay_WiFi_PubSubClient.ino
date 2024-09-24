///=== DHT ==== start ========
#include "DHT.h"

#define DHTPIN D6
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);
///=== DHT === end ===========

////=== Wi-Fi === start =================
#include <ESP8266WiFi.h>

const char* ssid     = "Yi";
const char* password = "3023230232";
////=== Wi-Fi === end ===================


///=== MQTT Pub. Sub. ========= start ===============
#include <PubSubClient.h>

// for MQTT Publish message - start
const char* mqttServer   = "140.128.99.71";
const int   mqttPort     = 1883;
const char* mqttUser     = "course";
const char* mqttPassword = "iot999";

// For MQTT Publish ===============
const char* mqttPublishTopic   = "ST019/TeamNN/Sensors";  // NN 改為組別號碼 01, 02, ...
unsigned long mqttLastPublishTime = 0;     //此變數用來記錄推播時間
unsigned long mqttPublishInterval = 2000; //每10秒推撥一次
// ================================

// For MQTT Subscribe ====
const char* mqttSubscribeTopic = "ST019/TeamNN/Relay";  // NN 改為組別號碼 01, 02, ...
String subMsg;
const int relay_pin = D1;
// =======================
///=== MQTT Pub. Sub. ========= end ===============

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(115200);

  // Check Relay(Low trigger) -- start --
  pinMode( relay_pin, OUTPUT);
  digitalWrite( relay_pin, LOW);
  delay(0.5*1000);
  digitalWrite( relay_pin, HIGH);
  // Check relay -- end --

  //=== DHT11 init ===//
  Serial.println("DHT11 measuring...");
  dht.begin();
  
  WifiConnect();  //開始WiFi連線

  MQTTConnect();  //開始MQTT連線
}

void loop() {
  if (WiFi.status() != WL_CONNECTED)  WifiConnect();    //如果WiFi連線中斷，則重啟WiFi連線
  if (!mqttClient.connected())        MQTTConnect();    //如果MQTT連線中斷，則重啟MQTT連線

  // MQTT Publish =================================================
  unsigned long now = millis();
  if ( now - mqttLastPublishTime > mqttPublishInterval ) {
    mqttLastPublishTime = now;

    //// Get sensor data...
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    Serial.print("Temperature: ");  Serial.print(t);  Serial.print(" °C\t");
    Serial.print("Humidity: ");     Serial.print(h);  Serial.println(" %");
    //// Get sensor data ... end....

    ////=== Package publish message ==== Method 1 === start =============
    //// format :   {"Temperature":25.6,"Humidity":56.0}
    char msg[50];
    sprintf(msg, "{\"Temperature\":%4.2f,\"Humidity\":%4.2f}", t, h);
    Serial.print("Publish message: ");   Serial.println(msg);
    if (mqttClient.publish(mqttPublishTopic, msg) == true) {
      Serial.print("Success publish message[");
      Serial.print(mqttPublishTopic);
      Serial.print("] ");
      Serial.println(msg);      
    } else {
      Serial.println("Error sending message");
    }  
  }
  // ==============================================================

  mqttClient.loop();  //更新訂閱狀態
  delay(0.05*1000);
}

//開始WiFi連線
void WifiConnect() {     //connect to WiFi AP
  delay(0.01*1000);
  Serial.printf("Connecting to %s ", ssid);

  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(0.5*1000);
    Serial.print(".");
  }
  Serial.println("Wi-Fi Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
//  Serial.print("MAC address: ");
//  Serial.println(WiFi.macAddress());
}

//開始MQTT連線
void MQTTConnect() {
  mqttClient.setServer( mqttServer, mqttPort);
  mqttClient.setCallback(mqttCallback);

  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");

    String  mqttClientID = "esp8266-" + String(random(1000000, 9999999)); //以亂數為ClietID
    if (mqttClient.connect(mqttClientID.c_str(), mqttUser, mqttPassword)) {
      Serial.println("MQTT已連線");     //連結成功，顯示「已連線」。
      //  // Once connected, publish an announcement...
      //  mqttClient.publish( mqttPublishTopic, "Reconnected: Hello World~~");
      //  // ... and resubscribe
      mqttClient.subscribe(mqttSubscribeTopic);  //訂閱SubTopic1主題
    } else {
      //若連線不成功，則顯示錯誤訊息，並重新連線
      Serial.print("MQTT連線失敗,狀態碼=");
      Serial.println(mqttClient.state());
      Serial.println("五秒後重新連線");
      delay(5*1000);
    }
  }
}

//接收到訂閱時
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String payloadString = "";
  // 將收到的訊息 payload 轉存成字串 
  for (int i = 0; i < length; i++) {
    payloadString = payloadString + (char)payload[i];
  }
  Serial.println(payloadString);

  // 判斷收到的訊息 payload 是否可 Trigger on/off the Relay
  if ( payloadString == "On" ) {
    digitalWrite( relay_pin, LOW);
    Serial.println("Ralay Triger On !!");
  } else if ( payloadString == "Off" ) {
    digitalWrite( relay_pin, HIGH);
    Serial.println("Ralay Triger Off !!");
  } else {
    Serial.println("Error message! Usage: { On| Off }");
  }
}
