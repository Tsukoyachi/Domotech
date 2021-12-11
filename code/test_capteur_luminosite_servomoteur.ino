#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

#include <Servo.h>
Servo monservo;
int pos = 180;
const int servomoteur = 7;
unsigned long previousTime =0;
const long interval_luminosite = 1000;

void setup(){
  Serial.begin(9600);
  Wire.begin();
  lightMeter.begin();
  Serial.println(F("BH1750 Test begin"));
  monservo.attach(servomoteur);
}


void loop() {
  unsigned long currentTime = millis();
  if ( currentTime - previousTime >= interval_luminosite){
    previousTime = currentTime;
    float lux = lightMeter.readLightLevel();
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");

    if(lux >= 200) {
      monservo.write(180);
    }
    else{
      monservo.write(0);
    }
  }
}
