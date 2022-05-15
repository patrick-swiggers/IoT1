#ifndef MY_CONN_H
#define MY_CONN_H
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>


void makeWiFiConnection(String accesspoint, String password);
void test();

#endif
