#include <Wire.h>             // I2C
#include <Adafruit_Sensor.h>  // Unified sensor library
#include <Adafruit_GFX.h>     // Core graphics
#include <Adafruit_SSD1306.h> // OLED display
#include "DHT.h"              // Humidity & Temperature
#include <BH1750.h>           // Light sensor
#include <WiFi.h>             // Wifi protocol
#include <InfluxDbClient.h>   // InfluxDB communication
#include <InfluxDbCloud.h>    // InfluxDB certificate
#include "esp_task_wdt.h"     // Watchdog

// === WiFi and InfluxDB settings ===
#define WIFI_SSID       "PosteMobile-79159793"
#define WIFI_PASSWORD   "4PF9yedTc2FdN5kzfu4EuSQk"
#define INFLUXDB_URL    "http://informatica-iot.freeddns.org:8086/"
#define INFLUXDB_ORG    "uniurb"
#define INFLUXDB_BUCKET "esercitazioni"
#define INFLUXDB_TOKEN  "qt5kHYb2Wwg19lbGEOe3BmVJhlMf6ZxfDu_Z7Lrhiiiv9FTEGKiD_pJzkDa_qSlUPLPm0zI-1yE6THb4kg-0kA=="
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN,
                      InfluxDbCloud2CACert); // InfluxDB instance
Point sensors("corso_IoT"); // Data point

// === OLED display ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// === Humidity & Temperature sensor ===
#define DHTPIN 14    // Connect DHT11 to GPIO14 pin
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// === Light sensor ===
BH1750 lightMeter;



void setup() {
  Wire.begin(21, 22); // I2C pins
  Serial.begin(115200); // Serial console
  analogReadResolution(12); // ESP32 ADC on 12 bit (0-4095)

  // Configure the watchdog to reboot the CPU if not reset within 20 seconds
  esp_task_wdt_config_t config = {
    .timeout_ms = 20* 1000,    //  20 seconds
    .trigger_panic = true,     // Trigger panic == reset CPU (reboot)
  };
  esp_task_wdt_reconfigure(&config); // Configure WDT
  enableLoopWDT(); // Enable WDT in loop()

  // --- OLED display ---
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED error")); 
    for(;;);
  }
  Serial.println("OLED initialized");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("OLED ready");

  // --- WiFi connection ---
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { // Wait for connection
    delay(500);
    Serial.print(".");
    display.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  Serial.print("RRSI: "); Serial.println(WiFi.RSSI());
  display.println("");
  display.println("WiFi connected");
  display.print("IP: "); display.println(WiFi.localIP());
  display.print("RSSI: "); display.println(WiFi.RSSI());
  
  // --- InfluxDB connection ---
  sensors.addTag("host", "ESP_LEOPIZZI");
  sensors.addTag("location", "Casarano (LE)");
  sensors.addTag("room", "Stanza");
  if(client.validateConnection()){ // Test connection
    Serial.println("Connected to influxDB");
    display.println("InfluxDB connected");
  }else {
    Serial.println("Connection failed!");
    display.println("InfluxDB error");
    Serial.println(client.getLastErrorMessage());    
  }  

  // --- Light sensor ---
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &Wire)) {
    Serial.println("BH1750 initialized");
    display.println("BH1750 ready");
  } else {
    Serial.println("BH1750 error");
    display.println("BH1750 error");
  }
  
  // --- Humidity & Temperature sensor ---
  dht.begin();
  Serial.println("DHT initialized");
  display.println("DHT ready");

  display.display();
  delay(5000);
}


void loop() {
  display.clearDisplay();
  display.setCursor(0,0);

  // --- Humidity & Temperature sensor ---
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    display.println("DHT11 Error");
  } else {
    display.print("Temp: "); display.print(t); display.println(" C");
    display.print("Umid: "); display.print(h); display.println(" %");
  }

  // --- Light sensor ---
  float lux = lightMeter.readLightLevel();
  display.print("Luce: "); display.print(lux); display.println(" lx");

  // --- Send data to InfluxDB ---
  sensors.clearFields();
  sensors.addField("temperature", t);
  sensors.addField("humidity", h);
  sensors.addField("light", lux);

  if(!client.writePoint(sensors)){
    Serial.print("InfluxDB write failed ");
    Serial.println(client.getLastErrorMessage());
    while(1); // In case of error, stop the program
  }  

  display.display();
  delay(1000); // refresh 20Hz
}

