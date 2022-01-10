#include <Servo.h>
#include <BH1750.h>
#include "DHT.h"
#define DHTPIN 2 //Pin sur lequel le capteur DHT envoie ses informations
#define DHTTYPE DHT22 // DHT22 comme notre composant AM2302 DHT22
#define VIN 5 // V power voltage of photoresistor
#define R 10000 //ohm resistance value

BH1750 lightMeter1(0x23); //capteur lumière intérieur
DHT dht(DHTPIN, DHTTYPE);

#include <Servo.h>
Servo myservo;
int pos = 0;

const int servomoteur = 9;
const int led_chauffage = 3;
const int MQ2 = 0;
int ppm = 0;
float temperature;
float thermostat = 20; //valeur seuil en degré celsius
float lux_int;
int lux_ext;
int norme_ppm = 1000;
int norme_particule = 2000;
int val_lux_ext;

boolean chauffage = false;
boolean fenetre = false;
boolean lumiere = false;

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
    Serial.print("CO2 : ");
    Serial.print(ppm);
    Serial.print(" ppm ");

    temperature = dht.readTemperature();
    Serial.print(F("| Temperature: "));
    Serial.print(temperature);
    Serial.print(F("°C "));

    lux_int = lightMeter1.readLightLevel();
    Serial.print("| intérieur : ");
    Serial.print(lux_int);
    Serial.print(" lx ");
    lux_ext = analogRead(A1); // Un entier pour contenir une valeur variant de 0 à 1023
    val_lux_ext = sensorRawToPhys(lux_ext); 
    Serial.print("| extérieur : ");
    Serial.print(val_lux_ext);
    Serial.print(" lx");

    //on effectue la mesure du capteur de particule dont la valeur qui nou intéresse sera stocké dans la variable concentration
    
    duration = pulseIn(capteur_particules, LOW);
    lowpulseoccupancy = lowpulseoccupancy+duration;
 
    if ((millis()-starttime) > sampletime_ms)//if the sampel time == 30s
    {
        ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
        concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
        lowpulseoccupancy = 0;
        starttime = millis();
    }
    Serial.print(" | Particules : ");
    Serial.print(concentration);
    Serial.print(" pcs/L ");
    //partie pour déterminer les actions exécutée par le système
    if (temperature >= thermostat){
      chauffage = false;
      if (ppm >= norme_ppm || concentration >= norme_particule) {
        fenetre = true;
        }
      }
    else {
      if (fenetre) {
        fenetre = false;
        chauffage = true;
        }
      }
    if (lux_int >= 200){
        if (val_lux_ext >= 200){
          if (!chauffage){
            fenetre = true;
            lumiere = false;
            }
          }
      }
    else {
      if (val_lux_ext >= 200){
         if (chauffage){
            lumiere = true;
            }
         else {
            fenetre = true;
            }
          }
        else {
          lumiere = true;
          }
      }
    Serial.print("| lumière : ");
    Serial.print(lumiere);
    Serial.print("| fenetre : ");
    Serial.print(fenetre);
    Serial.print("| chauffage : ");
    Serial.println(chauffage);
      
}

int sensorRawToPhys(int raw){ //Méthode pour convertir la valeur en ohm de la photorésistance en lumen (à tester mais semble fonctionner)
  // Conversion rule
  float Vout = float(raw) * (VIN / float(1023));// Conversion analog to voltage
  float RLDR = (R * (VIN - Vout))/Vout; // Conversion voltage to resistance
  int phys=500/(RLDR/1000); // Conversion resitance to lumen
  return phys;
}
