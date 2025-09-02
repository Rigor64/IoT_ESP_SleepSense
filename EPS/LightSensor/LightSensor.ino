#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

void setup(){
  Serial.begin(115200);
  Wire.begin(4, 5); // SDA=4, SCL=5
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0)) {
    Serial.println("BH1750 initialized");
  } else {
    Serial.println("Error initializing BH1750");
  }
}

void loop(){
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  delay(1000);
}