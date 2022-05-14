#include <ESP8266WiFi.h>
#include "my_conn.h"


void makeWiFiConnection(String accesspoint, String password) {
  WiFi.mode(WIFI_STA); // De ESP is een client
  WiFi.begin(accesspoint, password);
  Serial.println();
  Serial.println("[INFO] WiFi Verbinding maken");
  while (WiFi.status() != WL_CONNECTED)   //probeer opnieuw zolang het niet lukt
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("[INFO] Wifi Verbonden, IP adres: ");
  Serial.println(WiFi.localIP());
}

void test()
{
  Serial.println("TEST");
}
