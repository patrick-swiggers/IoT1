/* 
 *  Patrick Swiggers, mei/juni 2022, (CVO-IoT)
 *  
 *  Oef 9.5: Temperatuur en vochtigheid naar een TCP Server 
 *  
 *  Maak de schakeling met bijhorende sketch die het volgende doet:
 *  De ESP is een TCP Client die naar een TCP Server op een PC verbindt 
 *  (gebruik de applicatie “packet sender”). 
 *  Wanneer vanop de PC een willekeurige String wordt opgestuurd, 
 *  worden de temperatuur en vochtigheid opgevraagd en naar de PC verzonden.
 *  Tijdens de meting gaat er een LED branden bij het ESP bordje.
*/

#include <ESP8266WiFi.h>
#include <DHT.h>
/* WiFi */
const char server[] = "192.168.100.105"; //vul hier het IP adres van jouw PC in!
const int port = 6000; // TCP poort op de Server (PC)
WiFiClient mijnClient; // dit is een TCP client die aangemaakt wordt
/* DHT */
const int dhtPin = 12; // GPIO pin voor de DHT sensor
DHT dht(dhtPin, DHT11);
float h, t;
/* led */
int ledPin = 4; // D2(gpio4)

void setup() {
  // de setup functie wordt een enkele keer uitgevoerd
  {
    Serial.begin(115200);
    makeWiFiConnectie("dlink-8914", "<password>"); //maak de verbinding met het WiFI access punt
    dht.begin();
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW); // Zet ledPin uit bij aanvang
  }
}
void loop() {
  // the loop functie herhaalt zich in een oneindige lus
  if (!mijnClient.connected()) {
    if (mijnClient.connect(server, port)) {
      Serial.println("[INFO] TCP verbinding tot stand gebracht");
    }
    else {
      Serial.println("[INFO] TCP verbinding mislukt");
    }
  }
  if (mijnClient.connected()) {
    if (mijnClient.available()) {
      Serial.println("[INFO] Data ontvangen");
      String s = mijnClient.readStringUntil('\r');
      Serial.println(s);
      /* Lees DHT */
      digitalWrite(ledPin, HIGH); // zet ledPin aan
      h = dht.readHumidity(); // lees de vochtigheid in
      t = dht.readTemperature(); // lees temperatuur in Celsius (default)
      /* Stuur naar Server om te tonen */
      mijnClient.print("Vochtigheid: ");
      mijnClient.print(h);
      mijnClient.print("% Temperatuur: ");
      mijnClient.print(t);
      mijnClient.print("°C ");
      mijnClient.println();
      delay(2000); // laat ledPin nog een seconde branden om te kunnen zien
      digitalWrite(ledPin, LOW); // zet ledPin terug uit
    }
  }
  delay(2000);
}
void makeWiFiConnectie(String accesspoint, String wachtwoord) {
  const byte ledWiFi = 2; //GPIO2
  pinMode(ledWiFi, OUTPUT);
  digitalWrite(ledWiFi, LOW);
  Serial.println();
  WiFi.mode(WIFI_STA); // De ESP is een client
  WiFi.begin(accesspoint, wachtwoord);
  Serial.print("[INFO] WiFi Verbinding maken");
  while (WiFi.status() != WL_CONNECTED) //probeer opnieuw zolang het niet lukt
  {
    delay(500);
    Serial.print(".");
    digitalWrite(ledWiFi, not(digitalRead(ledWiFi)));
  }
  Serial.println();
  digitalWrite(ledWiFi, HIGH);
  Serial.print("[INFO] Wifi Verbonden, IP adres: ");
  Serial.println(WiFi.localIP());
}
