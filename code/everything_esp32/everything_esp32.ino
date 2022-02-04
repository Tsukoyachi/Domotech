#include <arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <ESP32Servo.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "DHT.h"
#include <WiFi.h>
#include <WebServer.h>

#define SDA 21
#define SCL 22
#define servo 18
#define DHTPIN 25
#define DHTTYPE DHT22
#define MQ2 36
#define Light_ext 34
#define VIN 3.3 // V power voltage
#define R 10000 //ohm resistance value

BH1750 lightMeter;
Servo myservo;
DHT dht(DHTPIN, DHTTYPE);
int pos = 0;
int led_chauffage = 23;
int led_lumiere = 15;
float thermostat = 22;
int norme_ppm = 1000;

float temperature;
float lux_int;
int lux_ext;
int mq2_value;

boolean chauffage = false;
boolean fenetre = false;
boolean lumiere = false;

const char *ssid = "Redmi Note 10 Pro"; // Enter your SSID here
const char *password = "31102002";      //Enter your Password here

WebServer server(80); // Object of WebServer(HTTP port, 80 is defult)

int sensorRawToPhys(int raw)
{
  // Conversion rule
  float Vout = float(raw) * (VIN / float(4095)); // Conversion analog to voltage
  float RLDR = (R * (VIN - Vout)) / Vout;        // Conversion voltage to resistance
  int phys = 500 / (RLDR / 1000);                // Conversion resitance to lumen
  return phys;
}

// Handle root url (/)
void handle_root()
{
  String page = "<!DOCTYPE html>";
  page += "<head>";
  page += "<html lang='fr'></html>";
  page += "<meta http-equiv='refresh' content='5' name='viewport' content='width=device-width, initial-scale=1' charset='UTF-8'/>";
  page += "</head>";
  page += "<body>";
  page += "<title>DomoTech</title>";
  page += "<h1>Site internet pour la gestion du projet arduino</h1>";
  page += "<div>";
  page += "    <h2><p>État actuel de l'algorithme</p></h2>";
  page += "    <ul>";
  page += "       <li><p>Etat chauffage :";page+=chauffage; page+= "</p></li>";
  page += "       <li><p>Etat fenêtre :";page+=fenetre; page+= " </p></li>";
  page += "        <li><p>Etat lumière :";page+=lumiere; page+= " </p></li>";
  page += "    </ul>";
  page += "</div>";
  page += "<div>";
  page += "   <h2><p>Valeur des différents capteur</p></h2>";
  page += "   <ul>";
  page += "       <li><p>Capteur température :";page+=temperature; page+= " °C</p></li>";
  page += "       <li><p>Capteur lumière intérieur :";page+=lux_int; page+= " lx</p></li>";
  page += "       <li><p>Capteur lumière intérieur :";page+=lux_ext; page+= " lx</p></li>";
  page += "       <li><p>Capteur CO2 :";page+=mq2_value; page+= " ppm</p></li>";
  page += "   </ul>";
  page += "</div>";
  page += "<i>Se serveur est hébergé sur ESP32</i>";
  page += "</body>";
  server.setContentLength(page.length());
  server.send(200, "text/html", page);
}

void handleNotFound()
{
  server.send(404, "text/plain", "404 : Not found!");
}

void setup()
{
  
  // put your setup code here, to run once:
  Wire.begin(SDA, SCL);
  Serial.begin(115200);
  pinMode(led_chauffage, OUTPUT);
  pinMode(led_lumiere, OUTPUT);
  lightMeter.begin();
  myservo.attach(servo);
  myservo.setPeriodHertz(50);
  myservo.write(0);
  Serial.println(F("DHTxx test!"));
  dht.begin();
  pinMode(MQ2, INPUT);
  Serial.println("Try Connecting to ");
  Serial.println(ssid);
  // Connect to your wi-fi modem
  WiFi.begin(ssid, password);

  // Check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected successfully");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP()); //Show ESP32 IP on serial

  server.on("/", handle_root);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  delay(100);
}

void loop()
{
  server.handleClient();
  // put your main code here, to run repeatedly:
  lux_int = lightMeter.readLightLevel();
  Serial.print("Luminosité intérieur : ");
  Serial.print(lux_int);
  Serial.print(" lx | ");
  int val = analogRead(34); // Un entier pour contenir une valeur variant de 0 à 1023
  lux_ext = sensorRawToPhys(val);
  Serial.print("Luminosité extérieur : ");
  Serial.print(lux_ext);
  Serial.print(" lx | ");
  temperature = dht.readTemperature();
  Serial.print("Température : ");
  Serial.print(temperature);
  Serial.print(" °C | ");
  mq2_value = analogRead(MQ2);
  Serial.print("MQ2 : ");
  Serial.print(mq2_value);
  Serial.println(" ppm | ");
  //partie pour déterminer les actions exécutée par le système
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
      chauffage = true;
    }
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
  Serial.print("| lumière : ");
  Serial.print(lumiere);
  Serial.print("| fenetre : ");
  Serial.print(fenetre);
  Serial.print("| chauffage : ");
  Serial.println(chauffage);
  delay(1000);

  myservo.write(150);
  delay(5000);
  myservo.write(95);
  delay(5000);
}
