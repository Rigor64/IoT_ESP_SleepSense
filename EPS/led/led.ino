#define LED 2

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  Serial.println("Programma avviato, LED Blink...");

}

void loop() {
  digitalWrite(LED, HIGH);
  Serial.println("LED acceso");
  delay(1000);

  digitalWrite(LED, LOW);
  Serial.println("LED spento");
  delay(1000);

}
