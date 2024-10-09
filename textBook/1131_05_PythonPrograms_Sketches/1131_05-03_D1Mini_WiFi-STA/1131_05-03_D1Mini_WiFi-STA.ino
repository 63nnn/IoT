#include <ESP8266WiFi.h>

const char* ssid     = "Yi";
const char* password = "3023230232";

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.printf("Connecting to %s ", ssid);

  WiFi.mode(WIFI_STA);  // 四種之一：WIFI_OFF、WIFI_STA、WIFI_AP、WIFI_AP_STA
  //* 若要指定IP位址，請自行在此加入WiFi.config()。
  // WiFi.config(IPAddress(192,168,1,50),    // IP位址
  //             IPAddress(192,168,1,1),     // 閘道（gateway）位址
  //             IPAddress(255,255,255,0));  // 子網路遮罩（subnetmask）
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(0.5*1000);
    Serial.print(".");
  }
  Serial.println("Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {}
