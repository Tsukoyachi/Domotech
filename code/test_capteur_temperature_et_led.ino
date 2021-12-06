#include "DHT.h"
#define DHTPIN 2 //Pin sur lequel le capteur envoie ses informations
#define DHTTYPE DHT22 // DHT22 comme notre composant AM2302 DHT22

DHT dht(DHTPIN, DHTTYPE);

const int led_chauffage = 3;

void setup() {
  // put your setup code here, to run once:
  pinMode(led_chauffage,OUTPUT);
  Serial.begin(9600);
  Serial.println(F("DHTxx Test!"));

  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  float t = dht.readTemperature(); //lit la température en degré celsius par défaut
  //boucle if qui s'exécute si le capteur n'a pas réussi à lire la température
  if (isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor"));
    return;
  }
  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.println(F("°C"));

  if(t<= 20){
    digitalWrite(led_chauffage,LOW); //le chauffage s'allume sous la barre du thermostat
    }
  else{
    digitalWrite(led_chauffage,HIGH); //le chauffage d'éteint en dépassant le thermostat
  }
}
