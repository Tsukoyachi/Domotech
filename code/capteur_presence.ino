const int buttonPin = 2; // broche du capteur PIR

int buttonState = 0; // etat de la sortie du capteur
 
void setup()
{Serial.begin(9600);
  pinMode(buttonPin, INPUT); //la broche du capteur est mise en entree
}
 
void loop()
{
  buttonState = digitalRead(buttonPin);//lecture du capteur
  Serial.println(buttonState);

  
  
}
