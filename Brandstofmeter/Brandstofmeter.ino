/*  Cursist: Patrick Swiggers
    Cursus:  Internet of Things (IoT)
    Datum:   februari 2022
    Oef 6.11: brandstofmeter
    Ontwerp een schakeling met bijhorende software , gebruik makend van een potentiometer
    en 3 LEDs (rood, geel groen) waarbij de LED het niveau van de brandstoftank weergeeft.
        - Groen is nog voldoende brandstof (>75%)
        - Geel is gemiddeld brandstof (25-75%)
        - Rood is laag brandstof niveau (<25%)
*/
const int inputPin = A0; // Analoge input om potentiometer op aan te sluiten
int ledGreen = 5;    //  D1(gpio5)
int ledYellow = 4;   //  D2(gpio4)
int ledRed = 0;      //  D3(gpio0)
int value;
int brandstof;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(inputPin, INPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledRed, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  value = analogRead(inputPin);   // Lees van A0
  brandstof = map(value, 0, 1024, 0, 100);   // Map hoeveelheid liter tussen 0 en 100
  Serial.println(String(brandstof) + " liter");   // Geef weer hoeveel liter
  if (brandstof < 25) {
    digitalWrite(ledRed, HIGH); // ROOD
    digitalWrite(ledYellow, LOW);
    digitalWrite(ledGreen, LOW);
  } else if (brandstof > 75) {
    digitalWrite(ledRed, LOW);
    digitalWrite(ledYellow, LOW);
    digitalWrite(ledGreen, HIGH); // GROEN
  } else {
    digitalWrite(ledRed, LOW);
    digitalWrite(ledYellow, HIGH); // GEEL
    digitalWrite(ledGreen, LOW);
  }

  delay(300);
}
