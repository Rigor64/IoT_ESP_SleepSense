#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define MIC_PIN 34   // collegare OUT del MAX4466 al pin GPIO34
#define ADC_MAX 4095
#define MID_VALUE 2048  // centro ADC ~1.65V

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // ESP32 ADC a 12 bit (0-4095)
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();
}

void loop() {
  int micValue = analogRead(MIC_PIN);
  
  // calcola ampiezza rispetto al centro
  int amplitude = abs(micValue - MID_VALUE);

  // scala l’ampiezza alla larghezza del display (0–128)
  int barLength = map(amplitude, 0, ADC_MAX/2, 0, SCREEN_WIDTH);

  Serial.print(micValue); Serial.print("\t"); Serial.println(amplitude);

  // disegna barra
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Mic Level:");
  display.fillRect(0, 16, barLength, 10, SSD1306_WHITE);
  display.display();

  delay(50); // refresh 20Hz
}

