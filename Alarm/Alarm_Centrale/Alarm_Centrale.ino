/*
   ALARM
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "my_conf.h"
#include "my_conn.h"

/* Sketch */
bool alarmAAN = false;
bool alarmBeep = false;
const int zoemerPin  = 4; //D2 GPIO4

/* MQTT - WiFi */
WiFiClient mijnTCPClient;
PubSubClient mijnMQTT;

void mijnCallback(char* topic, byte* payload, unsigned int lengte) {
  String onderwerp = topic;
  Serial.print("[INFO] ontvangen bericht met als onderwerp: ");
  Serial.println(topic);
  Serial.print("[INFO] payload: ");
  Serial.println(payload[0]); // ascii 48 => 0 , 49 => 1

  /* ALarm Aan */
  if (onderwerp == MQTT_ALARM_AAN_TOPIC && payload[0] == 49) {
    alarmAAN = true;
    mijnMQTT.publish(MQTT_ALARM_AAN_RESPONSE_TOPIC, "Alarm Aan");
    Serial.println("[INFO] Alarm staat aan");
  };

  /* ALarm Uit */
  if (onderwerp == MQTT_ALARM_AAN_TOPIC && payload[0] == 48) {
    alarmAAN = false;
    alarmBeep = false;
    digitalWrite(zoemerPin, LOW);
    mijnMQTT.publish(MQTT_ALARM_AAN_RESPONSE_TOPIC, "Alarm Uit");
    mijnMQTT.publish(MQTT_ALARM_BEEP_TOPIC, "0");
    mijnMQTT.publish(MQTT_ALARM_DETECTIE_TOPIC, "0");
    Serial.println("[INFO] Alarm staat uit");
  };

  /* Alarm Beeper Aan */
  if (onderwerp == MQTT_ALARM_BEEP_TOPIC && payload[0] == 49) {
    if (alarmAAN == true) {
      alarmBeep = true;
      Serial.println("[INFO] Beeper staat aan");
    } else {
      // Als het alarm niet aanstaat , zet beeper terug naar uit
      mijnMQTT.publish(MQTT_ALARM_BEEP_TOPIC, "0");
    }
  };

  /* Alarm Beeper Uit */
  if (onderwerp == MQTT_ALARM_BEEP_TOPIC && payload[0] == 48) {
    alarmBeep = false;
    Serial.println("[INFO] Beeper staat uit");
  };

  /* Alarm detectie */
  if (onderwerp == MQTT_ALARM_DETECTIE_TOPIC && payload[0] == 49) {
    if (alarmAAN == true && alarmBeep == true) {
      Serial.println("[INFO] Detectie");
      digitalWrite(zoemerPin, HIGH);
    }
  };
  
  /* Alarm geen detectie */
  if (onderwerp == MQTT_ALARM_DETECTIE_TOPIC && payload[0] == 48) {
    Serial.println("[INFO] Geen detectie");
    digitalWrite(zoemerPin, LOW);
  };
}


/*
   Setup
*/
void setup() {
  // put your setup code here, to run once:
  delay(100);
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
  makeMQTTSubscribe(MQTT_ALARM_DETECTIE_TOPIC);

  /* MQTT Opstart */
  alarmAAN = false;
  alarmBeep = false;
  mijnMQTT.publish(MQTT_ALARM_AAN_TOPIC, "0");
  mijnMQTT.publish(MQTT_ALARM_AAN_RESPONSE_TOPIC, "0");
  mijnMQTT.publish(MQTT_ALARM_BEEP_TOPIC, "0");

  /* Sketch Opstart */
  pinMode(zoemerPin, OUTPUT);
  digitalWrite(zoemerPin, LOW);
  Serial.println("[INFO] Alarm start");
}

/*
   Loop
*/
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
    makeMQTTSubscribe(MQTT_ALARM_AAN_TOPIC);
    makeMQTTSubscribe(MQTT_ALARM_BEEP_TOPIC);
    makeMQTTSubscribe(MQTT_ALARM_DETECTIE_TOPIC);
  }
}

void makeMQTTConnection() {
  Serial.println("[INFO] Verbinden met MQTT Broker...");
  if (mijnMQTT.connect(MQTT_CLIENT_ID_CENTRALE)) {
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
