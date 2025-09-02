#include <Wire.h> // I2C library
#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_SSD1306.h> // OLED display
#include "DHT.h" // Include DHT sensor library (Humidity and Temperature)

// OLED display 
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1  
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 

// Humidity and Temperature sensor
#define DHTPIN 14 // Connected to GPIO5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation falied")); 
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Test graphics ...");
  display.display();
  delay(2000); // Pause for 2 seconds
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  display.clearDisplay();
  display.setCursor(0,0);

  if (isnan(h) || isnan(t)) {
    display.println("Errore lettura DHT11");
  } else {
    display.print("Temp: "); display.print(t); display.println(" C");
    display.print("Umid: "); display.print(h); display.println(" %");
  }
  display.display();
  delay(2000);
}
