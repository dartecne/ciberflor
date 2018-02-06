
void setup() {
}

void loop() {
  int sensorReading = analogRead(A0);
  int thisPitch = map(sensorReading, 400, 1000, 120, 1500);

  tone(10, thisPitch, 10);
  delay(1);        
}
