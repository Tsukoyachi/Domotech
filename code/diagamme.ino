#include <Servo.h>
#include <BH1750.h>
#include "DHT.h"
#define DHTPIN 2 //Pin sur lequel le capteur envoie ses informations
#define DHTTYPE DHT22 // DHT22 comme notre composant AM2302 DHT22

BH1750 lightMeter1(0x23); //capteur lumière intérieur
BH1750 lightMeter2(0x5C); //capteur lumière extérieur
DHT dht(DHTPIN, DHTTYPE);

#include <Servo.h>
Servo myservo;
int pos = 0;

const int servomoteur = 9;
const int led_chauffage = 3;
const int MQ2 = 0;
int ppm = 0;
float temperature;
float lux_int;
float lux_ext;

boolean chauffage = false;
boolean fenetre = false;

//partie pour capteur particules
int capteur_particules = 8;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;//sampe 30s ;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

void setup() {
  pinMode(led_chauffage,OUTPUT);
  Serial.begin(9600);
  lightMeter1.begin();
  lightMeter2.begin();
  Serial.println(F("BH1750 Test begin"));
  Serial.println(F("DHTxx Test!"));
  dht.begin();
  pinMode(MQ2,INPUT);
  pinMode(capteur_particules,INPUT);
  starttime = millis();
}

void loop() {

    //on effectue toutes les mesures sauf celle du capteur de particules
    ppm = analogRead(MQ2);
    Serial.print("ppm : ");
    Serial.println(ppm);

    temperature = dht.readTemperature();
    Serial.print(F("Temperature: "));
    Serial.print(temperature);
    Serial.println(F("°C"));

    lux_int = lightMeter1.readLightLevel();
    lux_ext = lightMeter2.readLightLevel();

    Serial.println(lux_int);
    Serial.println(lux_ext);
    Serial.println("ok");
    delay(2000);

    //on effectue la mesure du capteur de particule dont la valeur qui nou intéresse sera stocké dans la variable concentration
    
    duration = pulseIn(capteur_particules, LOW);
    lowpulseoccupancy = lowpulseoccupancy+duration;
 
    if ((millis()-starttime) > sampletime_ms)//if the sampel time == 30s
    {
        ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
        concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
        Serial.print(lowpulseoccupancy);
        Serial.print(",");
        Serial.print(ratio);
        Serial.print(",");
        Serial.println(concentration);
        lowpulseoccupancy = 0;
        starttime = millis();
    }
}
