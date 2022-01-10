#include <Servo.h>
#include <BH1750.h>

BH1750 lightMeter;

#include <Servo.h>
Servo myservo;
int pos = 0;

const int servomoteur = 9;
unsigned long previousTime =0;
const long interval_luminosite = 1000;

void setup(){
  Serial.begin(9600);
  lightMeter.begin();
  Serial.println(F("BH1750 Test begin"));
  myservo.attach(servomoteur);
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
      Serial.println(">=200");
      if(pos<=0){
        for (pos = 0; pos <= 180; pos += 10) { // goes from 0 degrees to 180 degrees
        // in steps of 10 degree
           myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(50);  
        }// waits 15 ms for the servo to reach the position
      }
    }
    else{
      Serial.println("<200");
      if (pos>=180){
       for (pos = 180; pos >= 0; pos -= 10) { // goes from 180 degrees to 0 degrees
         myservo.write(pos);              // tell servo to go to position in variable 'pos'
         delay(50);                       // waits 15 ms for the servo to reach the position
        }
      }
    }
  }
}
