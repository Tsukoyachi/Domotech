
#define VIN 5 // V power voltage
#define R 10000 //ohm resistance value

void setup() { 
  Serial.begin(9600);
  } 
void loop() { 
  int val = analogRead(A1); // Un entier pour contenir une valeur variant de 0 à 1023
  int val_lux = sensorRawToPhys(val); 
  Serial.println(val);
  Serial.print(val_lux);
  Serial.println(" lx");
  delay(1000); 
  }

 int sensorRawToPhys(int raw){
  // Conversion rule
  float Vout = float(raw) * (VIN / float(1023));// Conversion analog to voltage
  float RLDR = (R * (VIN - Vout))/Vout; // Conversion voltage to resistance
  int phys=500/(RLDR/1000); // Conversion resitance to lumen
  return phys;
}

/*
// Potentiometer is connected to GPIO 34 (Analog ADC1_CH6) 
const int potPin = 34;

// variable for storing the potentiometer value
int potValue = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  // Reading potentiometer value
  potValue = analogRead(potPin);
  Serial.println(potValue);
  delay(500);
}*/
