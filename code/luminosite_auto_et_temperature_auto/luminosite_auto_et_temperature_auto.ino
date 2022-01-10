#include <Servo.h>
#include <BH1750.h>
#include "DHT.h"
#define DHTPIN 2 //Pin sur lequel le capteur envoie ses informations
#define DHTTYPE DHT22 // DHT22 comme notre composant AM2302 DHT22

BH1750 lightMeter;
DHT dht(DHTPIN, DHTTYPE);

#include <Servo.h>
Servo myservo;
int pos = 0;

const int servomoteur = 9;
unsigned long previousTimeLuminosite=0;
unsigned long previousTimeTemperature=0;
const long interval_luminosite = 1000;
const int led_chauffage = 3;
unsigned long previousTime =0;
const long interval_temperature = 2000;

void setup(){
  pinMode(led_chauffage,OUTPUT);
  Serial.begin(9600);
  lightMeter.begin();
  Serial.println(F("BH1750 Test begin"));
  myservo.attach(servomoteur);
  Serial.println(F("DHTxx Test!"));
  dht.begin();
}


void loop() {
  unsigned long currentTime = millis();
  if ( currentTime - previousTimeLuminosite >= interval_luminosite){
    previousTimeLuminosite = currentTime;
    float lux = lightMeter.readLightLevel();
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");

    if(lux >= 200) {
      Serial.println(">=200");
      if(pos <= 0){
      for (pos = 0; pos <= 180; pos++) { 
        myservo.write(pos); 
        delay(15); 
        }
      }
    }
    else{
      Serial.println("<200");
      if(pos >= 180){
      for ( pos = 180; pos >= 0; pos--) { 
        myservo.write(pos); 
        delay(15); 
      }
     }
    }
  }
  if ( currentTime - previousTimeTemperature >= interval_temperature){
    previousTimeTemperature = currentTime;
    float t = dht.readTemperature(); //lit la température en degré celsius par défaut
    //boucle if qui s'exécute si le capteur n'a pas réussi à lire la température
    if (isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor"));
      return;
    }
    Serial.print(F("Temperature: "));
    Serial.print(t);
    Serial.println(F("°C"));
  
    if(t<= 25){
      digitalWrite(led_chauffage,LOW); //le chauffage s'allume sous la barre du thermostat
      }
    else{
      digitalWrite(led_chauffage,HIGH); //le chauffage d'éteint en dépassant le thermostat
    }
  }
}
