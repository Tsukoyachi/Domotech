const int MQ2 = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(MQ2,INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int analogSensor = analogRead(MQ2);
  Serial.print(analogSensor);
  Serial.println(" ppm");
}
