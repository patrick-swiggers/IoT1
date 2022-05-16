/*
   ALARM
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "my_conf.h"
#include "my_conn.h"

/* Sketch */
const int detectMove = 4; //D2 GPIO4

/* MQTT - WiFi */
WiFiClient mijnTCPClient;
PubSubClient mijnMQTT;

void setup() {
  // put your setup code here, to run once:
  delay(500);
  Serial.begin(115200);
  
  /* Maak WiFi Connectie */
  makeWiFiConnection(WIFI_ACCESSPOINT, WIFI_PASSWORD);
  
  /* MQTT */
  mijnMQTT.setClient(mijnTCPClient);    // Associeer een onderliggende TCP laag
  mijnMQTT.setServer(MQTT_SERVER, MQTT_PORT); // Zet de Broker info
  makeMQTTConnection();
  
  /* Opstart */
  mijnMQTT.publish(MQTT_ALARM_DETECTIE_TOPIC, "Geen");
  pinMode(detectMove, INPUT);
  digitalWrite(detectMove, LOW);
  Serial.println("[INFO] Alarm start");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(300);

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[FOUT] Verbinding met Wifi verbroken");
    makeWiFiConnection(WIFI_ACCESSPOINT, WIFI_PASSWORD);
  }

  if (!mijnMQTT.loop()) { //onderhoud de verbinding en kijk voor binnenkomende berichten
    Serial.print("[FOUT] Verbinding met MQTT Broker verbroken. Foutcode ");
    Serial.println(mijnMQTT.state());
    makeMQTTConnection();
  }

  if (digitalRead(detectMove) == LOW) {
    Serial.println("[INFO] Geen detectie");
    mijnMQTT.publish(MQTT_ALARM_DETECTIE_TOPIC, "0");
  } else {
    Serial.println("[WARN] Detectie");
    mijnMQTT.publish(MQTT_ALARM_DETECTIE_TOPIC, "1");
  }
}

void makeMQTTConnection() {
  Serial.println("[INFO] Verbinden met MQTT Broker...");
  if (mijnMQTT.connect(MQTT_CLIENT_ID_NODE01)) {
    Serial.println("[INFO] Verbinding met MQTT Broker gelukt.");
  } else {
    Serial.print("[FOUT] Verbinding met MQTT Broker mislukt. Foutcode ");
    Serial.println(mijnMQTT.state());
  }
}
