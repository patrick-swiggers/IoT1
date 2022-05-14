/*
   ALARM
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <StreamString.h>
#include "my_conf.h"
#include "my_conn.h"

/* Sketch */
int alarmAAN = 0;
int alarmBeep = 0;
const int detectMove = 16; //D0 GPIO16
const int zoemerPin  = 05; //D1 GPIO5

/* MQTT - WiFi */
WiFiClient mijnTCPClient;
PubSubClient mijnMQTT;

void mijnCallback(char* topic, byte * payload, unsigned int lengte) {
  String onderwerp = topic;
  Serial.print("[INFO] ontvangen bericht met als onderwerp: ");
  Serial.println(topic);
  Serial.print("[INFO] payload: ");
  Serial.println(payload[0]);
  /* ALarm Aan */
  if (onderwerp == MQTT_ALARM_AAN_TOPIC && payload[0] == 49) {
    alarmAAN = 1;
    mijnMQTT.publish(MQTT_ALARM_AAN_RESPONSE_TOPIC, "Alarm Aan");
    Serial.println("[INFO] Alarm staat aan");
  };
  /* ALarm Uit */
  if (onderwerp == MQTT_ALARM_AAN_TOPIC && payload[0] == 48) {
    alarmAAN = 0;
    alarmBeep = 0;
    digitalWrite(zoemerPin, LOW);
    mijnMQTT.publish(MQTT_ALARM_DETECTIE_TOPIC, "0");
    mijnMQTT.publish(MQTT_ALARM_BEEP_TOPIC, "0");
    mijnMQTT.publish(MQTT_ALARM_AAN_RESPONSE_TOPIC, "Alarm Uit");
    Serial.println("[INFO] Alarm staat uit");
  };
  /* Alarm Beeper Aan */
  if (onderwerp == MQTT_ALARM_BEEP_TOPIC && payload[0] == 49) {
    if (alarmAAN == 1) {
      alarmBeep = 1;
      Serial.println("[INFO] Beeper staat aan");
    } else {
      // Als het alarm niet aanstaat , zet beeper terug naar uit
      mijnMQTT.publish(MQTT_ALARM_BEEP_TOPIC, "0");
    }
  /* Alarm Beeper Uit */
  };
  if (onderwerp == MQTT_ALARM_BEEP_TOPIC && payload[0] == 48) {
    alarmBeep = 0;
    Serial.println("[INFO] Beeper staat uit");
  };
}


/*
   Setup
*/
void setup() {
  // put your setup code here, to run once:
  delay(1000);
  Serial.begin(115200);

  /* Maak WiFi Connectie */
  makeWiFiConnection(WIFI_ACCESSPOINT, WIFI_PASSWORD);

  /* MQTT */
  mijnMQTT.setClient(mijnTCPClient);    // Associeer een onderliggende TCP laag
  mijnMQTT.setServer(MQTT_SERVER, MQTT_PORT); // Zet de Broker info
  mijnMQTT.setCallback(mijnCallback);
  makeMQTTConnection();
  makeMQTTSubscribe(MQTT_ALARM_AAN_TOPIC);
  makeMQTTSubscribe(MQTT_ALARM_BEEP_TOPIC);

  /* MQTT Opstart */
  alarmAAN = 0;
  alarmBeep = 0;
  mijnMQTT.publish(MQTT_ALARM_AAN_TOPIC, "0");
  mijnMQTT.publish(MQTT_ALARM_AAN_RESPONSE_TOPIC, "Alarm Uit");
  mijnMQTT.publish(MQTT_ALARM_BEEP_TOPIC, "0");
  mijnMQTT.publish(MQTT_ALARM_DETECTIE_TOPIC, "Geen");

  /* Sketch Opstart */
  pinMode(detectMove, INPUT_PULLUP);
  pinMode(zoemerPin, OUTPUT);
  digitalWrite(detectMove, LOW);
  digitalWrite(zoemerPin, LOW);

  Serial.println("[INFO] Alarm start");
}

/*
   Loop
*/
void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[FOUT] Verbinding met Wifi verbroken");
    makeWiFiConnection(WIFI_ACCESSPOINT, WIFI_PASSWORD);
  }

  if (!mijnMQTT.loop()) { //onderhoud de verbinding en kijk voor binnenkomende berichten
    Serial.print("[FOUT] Verbinding met MQTT Broker verbroken. Foutcode ");
    Serial.println(mijnMQTT.state());
    makeMQTTConnection();
    makeMQTTSubscribe(MQTT_ALARM_AAN_TOPIC);
    makeMQTTSubscribe(MQTT_ALARM_BEEP_TOPIC);
  }

  if (alarmAAN == 1) {
    if (digitalRead(detectMove) == LOW) {
      Serial.println("[INFO] Geen detectie");
      mijnMQTT.publish(MQTT_ALARM_DETECTIE_TOPIC, "Geen");
      digitalWrite(zoemerPin, LOW);
    } else {
      Serial.println("[WARN] Detectie");
      mijnMQTT.publish(MQTT_ALARM_DETECTIE_TOPIC, "Detectie");
      if (alarmBeep == 1) {
        digitalWrite(zoemerPin, HIGH);
      }
    }
  }
}


void makeMQTTConnection() {
  Serial.println("[INFO] Verbinden met MQTT Broker...");
  if (mijnMQTT.connect(MQTT_CLIENT_ID)) {
    Serial.println("[INFO] Verbinding met MQTT Broker gelukt.");
  } else {
    Serial.print("[FOUT] Verbinding met MQTT Broker mislukt. Foutcode ");
    Serial.println(mijnMQTT.state());
  }
}

void makeMQTTSubscribe(const char* topic) {
  if (strlen(topic) > 0) {
    if (mijnMQTT.subscribe(topic)) {
      Serial.print("[INFO] MQTT Subscribe uitgevoerd: ");
      Serial.println(topic);
    }
    else {
      Serial.print("[FOUT] MQTT Subscribe mislukt, Foutcode ");
      Serial.println(mijnMQTT.state());
      delay(1000);  //Als er iets fout gaat, wacht dan even
    }
  }
}
