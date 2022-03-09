#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>

#include <Wire.h>
#include <BH1750.h>
#include <ESP32Servo.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "DHT.h"
#include <WiFi.h>

const char *ssid = "Redmi Note 10 Pro";
const char *password = "Domotech";

AsyncWebServer server(80);

#define SDA 21
#define SCL 22
#define servo 17
#define DHTPIN 25
#define DHTTYPE DHT22
#define MQ2 36
#define Light_ext 34
#define VIN 3.3 // V power voltage
#define R 10000 // ohm resistance value

BH1750 lightMeter;
Servo myservo;
DHT dht(DHTPIN, DHTTYPE);
int pos = 0;
int led_chauffage = 23;
int led_lumiere = 15;
float thermostat = 19;
int norme_ppm = 1000;

float temperature = 0;
float lux_int = 0;
int lux_ext = 0;
int mq2_value = 0;

boolean chauffage = false;
boolean fenetre = false;
boolean lumiere = false;

boolean controle_fenetre = false;
boolean user_fenetre;
boolean controle_chauffage = false;
boolean user_chauffage;
boolean controle_lumiere = false;
boolean user_lumiere;

int sensorRawToPhys(int raw)
{
  // Conversion rule
  float Vout = float(raw) * (VIN / float(4095)); // Conversion analog to voltage
  float RLDR = (R * (VIN - Vout)) / Vout;        // Conversion voltage to resistance
  int phys = 500 / (RLDR / 1000);                // Conversion resitance to lumen
  return phys;
}

void setup()
{
  //----------------------Serial
  Serial.begin(115200);
  Serial.println("let's go !");
  //----------------------GPIO
  Wire.begin(SDA, SCL);
  pinMode(led_chauffage, OUTPUT);
  pinMode(led_lumiere, OUTPUT);
  lightMeter.begin();
  myservo.attach(servo);
  myservo.setPeriodHertz(50);
  myservo.write(0);
  Serial.println(F("DHTxx test!"));
  dht.begin();
  pinMode(MQ2, INPUT);
  //----------------------SPIFFS
  // Le SPIFFS est un gestionnaire de fichier en quelque sorte qui me permet de gérer mon html, mon css et mon script js
  if (!SPIFFS.begin())
  {
    Serial.println("Erreur SPIFFS...");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while (file)
  {
    Serial.print("File: ");
    Serial.println(file.name());
    file.close();
    file = root.openNextFile();
  }
  //----------------------Wifi
  Serial.println("Essaye de ce connecter au réseau : ");
  Serial.println(ssid);
  // Connexion au modem wifi
  WiFi.begin(ssid, password);

  // Vérfie si le wifi est bien fonctionnel (en général le moniteur affiche 3 points et passe à la suite
  // si ça fonctionne , sinon la connexion a échoué et il faut relancer le code)
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected successfully");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP()); // Affiche l'IP de l'ESP32 sur le moniteur série
  //----------------------Server
  // Pour faire simple, on gère un a un l'envoi des fichiers au serveur ainsi que l'intégralitédes requête du
  // script js pour update les valeurs et gérer les clics des boutons

  // envoi des fichiers
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });
  server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/w3.css", "text/css"); });
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/script.js", "text/javascript"); });
  server.on("/jquery-3.6.0.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/jquery-3.6.0.min.js", "text/javascript"); });
  // gestion bouton fenetre
  server.on("/on/fenetre", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              controle_fenetre = true;
              user_fenetre = true;
              request->send(200); });
  server.on("/off/fenetre", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              controle_fenetre = true;
              user_fenetre = false;
              request->send(200); });
  server.on("/reset/fenetre", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              controle_fenetre = false;
              user_fenetre = false;
              request->send(200); });
  // gestion bouton lumiere
  server.on("/on/lumiere", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              controle_lumiere = true;
              user_lumiere = true;
              request->send(200); });
  server.on("/off/lumiere", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              controle_lumiere = true;
              user_lumiere = false;
              request->send(200); });
  server.on("/reset/lumiere", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              controle_lumiere = false;
              user_lumiere = false;
              request->send(200); });
  // gestion bouton chauffage
  server.on("/on/chauffage", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              controle_chauffage = true;
              user_chauffage = true;
              request->send(200); });
  server.on("/off/chauffage", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              controle_chauffage = true;
              user_chauffage = false;
              request->send(200); });
  server.on("/reset/chauffage", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              controle_chauffage = false;
              user_chauffage = false;
              request->send(200); });
  // gestion changement thermostat

  server.on("/changeThermostat", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    if(request->hasParam("valeurThermostat", true))
    {
      String message;
      message = request->getParam("valeurThermostat", true)->value();
      thermostat = message.toFloat();
    }
    request->send(204); });

  // gestion récupération des booleans
  server.on("/booleanChauffage", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(chauffage)); });
  server.on("/booleanLumiere", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(lumiere)); });
  server.on("/booleanFenetre", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(fenetre)); });
  // gestion récupération des valeurs des capteurs
  server.on("/temperatureValue", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(temperature)); });
  server.on("/lumiereIntValue", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(lux_int)); });
  server.on("/lumiereExtValue", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(lux_ext)); });
  server.on("/mq2Value", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(mq2_value)); });
  // gestion des textes sur le statut actuel de la gestion des actions de la maison
  //  Par exemple pour la fenetre : Contrôlé par l'algorithme | ouverte | fermée
  server.on("/userLumiereString", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String etat_controle_lumiere = "Contrôlée par l'algorithme";
    if (controle_lumiere)
  {
    if (user_lumiere)
    {
      etat_controle_lumiere = "Allumée par utilisateur";
    }
    else
    {
      etat_controle_lumiere = "Eteinte par utilisateur";
    }
  }
    request->send(200, "text/plain", etat_controle_lumiere); });

  server.on("/userChauffageString", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String etat_controle_chauffage = "Contrôlé par l'algorithme";
    if (controle_chauffage)
  {
    if (user_chauffage)
    {
      etat_controle_chauffage = "Allumé par utilisateur";
    }
    else
    {
      etat_controle_chauffage = "Eteint par utilisateur";
    }
  }
    request->send(200, "text/plain", etat_controle_chauffage); });

  server.on("/userFenetreString", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String etat_controle_fenetre = "Contrôlée par l'algorithme";
    if (controle_fenetre)
  {
    if (user_fenetre)
    {
      etat_controle_fenetre = "Ouverte par utilisateur";
    }
    else
    {
      etat_controle_fenetre = "Fermée par utilisateur";
    }
  }
    request->send(200, "text/plain", etat_controle_fenetre); });

  server.on("/thermostatValue", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(thermostat)); });

  // Et enfin le lancement du serveur !
  server.begin();
  Serial.println("Serveur up and running !");
}

void loop()
{
  //Lecture des différents capteur et affichage de leur valeurs sur le moniteur série
  lux_int = lightMeter.readLightLevel();
  Serial.print("Luminosité intérieur : ");
  Serial.print(lux_int);
  Serial.print(" lx | ");
  int val = analogRead(Light_ext); // Un entier pour contenir une valeur variant de 0 à 4095
  lux_ext = sensorRawToPhys(val);
  Serial.print("Luminosité extérieur : ");
  Serial.print(lux_ext);
  Serial.print(" lx | ");
  float newTemperature = dht.readTemperature();
  if(!isnan(newTemperature)){
    temperature=newTemperature;
  }
  Serial.print("Température : ");
  Serial.print(temperature);
  Serial.print(" °C | ");
  mq2_value = analogRead(MQ2);
  Serial.print("MQ2 : ");
  Serial.print(mq2_value);
  Serial.println(" ppm | ");

  // L'ensemble des tests de mon algorithme pour savoir quel choix faire si l'utilisateur laisse l'algorithme gérer la maison.
  if (temperature >= thermostat)
  {
    chauffage = false;
    if (mq2_value >= norme_ppm)
    {
      fenetre = true;
    }
  }
  else
  {
    if (fenetre)
    {
      fenetre = false;
    }
    chauffage = true;
  }
  if (lux_int >= 200)
  {
    if (lux_ext >= 200)
    {
      if (!chauffage)
      {
        fenetre = true;
        lumiere = false;
      }
    }
  }
  else
  {
    if (lux_ext >= 200)
    {
      if (chauffage)
      {
        lumiere = true;
      }
      else
      {
        fenetre = true;
      }
    }
    else
    {
      lumiere = true;
    }
  }

  // Affichage des résultats de l'algorithme dans le moniteur série
  Serial.print("algorithme : lumière : ");
  Serial.print(lumiere);
  Serial.print("| fenetre : ");
  Serial.print(fenetre);
  Serial.print("| chauffage : ");
  Serial.println(chauffage);
  Serial.print("thermostat : ");
  Serial.println(thermostat);
  // Gestion ouverture et fermeture fenêtre en prenant en compte l'algorithme et les choix de l'utilisateur
  if (controle_fenetre)
  {
    if (user_fenetre && pos != 100)
    {
      for(pos;pos<=100; pos+=5){
        myservo.write(pos);
        delay(50);
      }
    }
    else if (!user_fenetre && pos != 0)
    {
      for(pos;pos>=0; pos-=5){
        myservo.write(pos);
        delay(50);
      }
    }
  }
  else
  {
    if (fenetre && pos != 100)
    {
      for(pos;pos<=100; pos+=5){
        myservo.write(pos);
        delay(50);
      }
    }
    else if (!fenetre && pos != 0)
    {
      for(pos;pos>=0; pos-=5){
        myservo.write(pos);
        delay(50);
      }
    }
  }

  // Gestion allumage et extinction de la lumière en prenant en compte l'algorithme et les choix de l'utilisateur
  if (controle_lumiere)
  {
    if (user_lumiere)
    {
      digitalWrite(led_lumiere, HIGH);
    }
    else
    {
      digitalWrite(led_lumiere, LOW);
    }
  }
  else
  {
    if (lumiere)
    {
      digitalWrite(led_lumiere, HIGH);
    }
    else
    {
      digitalWrite(led_lumiere, LOW);
    }
  }

  // Gestion allumage et extinction du chauffage en prenant en compte l'algorithme et les choix de l'utilisateur
  if (controle_chauffage)
  {
    if (user_chauffage)
    {
      digitalWrite(led_chauffage, LOW);
    }
    else
    {
      digitalWrite(led_chauffage, HIGH);
    }
  }
  else
  {
    if (chauffage)
    {
      digitalWrite(led_chauffage, LOW);
    }
    else
    {
      digitalWrite(led_chauffage, HIGH);
    }
  }
  delay(1000);
}
