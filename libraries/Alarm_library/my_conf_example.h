/* 
 * Rename file to my_conf.h
 */

#ifndef MY_CONF_H
#define MY_CONF_H

/* WiFi */
const String WIFI_ACCESSPOINT = ""; //iot1
const String WIFI_PASSWORD = ""; //cursusiot

/* MQTT settings */
const char MQTT_SERVER[] = "test.mosquitto.org";
const unsigned int MQTT_PORT = 1883; // Dit is de default MQTT poort
const char MQTT_CLIENT_ID_CENTRALE[] = ""; // dit is mijn LPN
const char MQTT_CLIENT_ID_NODE01[] = ""; // dit is mijn LPN

/* Mijn Topics */
const char MQTT_ALARM_AAN_TOPIC[] = "";
const char MQTT_ALARM_AAN_RESPONSE_TOPIC[] = "";
const char MQTT_ALARM_DETECTIE_TOPIC[] = "";
const char MQTT_ALARM_BEEP_TOPIC[] = "";

#endif
