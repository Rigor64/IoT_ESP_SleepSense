#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.println("\nI2C Scanner");
}

void loop() {
  byte error, address;
  int nDevices = 0;

  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("Device trovato all'indirizzo 0x");
      Serial.println(address, HEX);
      nDevices++;
    }
  }
  if (nDevices == 0)
    Serial.println("Nessun dispositivo I2C trovato\n");
  else
    Serial.println("Fine scansione\n");

  delay(5000); 
}
