/*
 *      SleepSense - Smart sleep monitoring system
 *
 *      OLED  Vcc    --- 3.3V
 *            SCL    --- 22
 *            SDA    --- 21
 *            gnd    --- GND
 * 
 *     DHT11  Vcc    --- 3.3V
 *            data   --- 14 
 *            gnd    --- GND 
 * 
 *    BH1750  Vcc    --- 3.3V
 *            SCL    --- 22
 *            SDA    --- 21
 *            gnd    --- GND
 * 
 *   MAX4466  Vcc    --- 3.3V
 *            out    --- 13
 *            gnd    --- GND
 * 
 *   MPU6050  Vcc    --- 3.3V
 *            SCL    --- 22
 *            SDA    --- 21
 *            gnd    --- GND
 *            AD0    --- GND
 * 
*/

#include <Wire.h>             // I2C
#include <Adafruit_Sensor.h>  // Unified sensor library
#include <Adafruit_GFX.h>     // Core graphics
#include <Adafruit_SSD1306.h> // OLED display
#include "DHT.h"              // Humidity & Temperature
#include <BH1750.h>           // Light sensor
#include <MPU6050_light.h>    // Accelerometer & Gyroscope
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

// === Microphone ===
#define MIC_PIN 34   // Connect MAX4466 to GPIO34 pin
#define ADC_MAX 4095 // ESP32 ADC max value
#define MID_VALUE 2048  // ADC middle value ~1.65V

// === Accelerometer & Gyroscope ===
MPU6050 mpu(Wire); 
const float ACC_THRESHOLD = 0.05;   // Minimum variation of acceleration (g) for moviment detection
const float GYRO_THRESHOLD = 5.0;  // Minimum variation of gyroscope (deg/s) for moviment detection

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
  sensors.addTag("location", "Lecce");
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

  // --- Accelerometer & Gyroscope ---
  byte status = mpu.begin();
  if (status != 0) {
    Serial.println("MPU noy found!");
    while(1);
  }
  Serial.print(F("MPU6050 status: ")); Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  delay(1000);
  mpu.calcOffsets(true,true);
  display.println("MPU6050 ready");
  
  display.println("MAX4466 ready");
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
    display.print("T:"); display.print(t); display.print("C "); display.print("H:"); display.print(h); display.println("%");
  }

  // --- Light sensor ---
  float lux = lightMeter.readLightLevel();
  display.print("Lux: "); display.print(lux); display.println(" lx");

  // --- Microphone ---
  int maxAmp = 0;
  for (int i = 0; i < 100; i++) {                             // Calculate the mean of multiple sampling
    int micValue = analogRead(MIC_PIN);
    int amplitude = abs(micValue - MID_VALUE);
    if (amplitude > maxAmp) maxAmp = amplitude;
    delayMicroseconds(200);                                   // Sampling rapido
  }
  int amplifiedAmp = maxAmp * 3;                              // Software amplification
  amplifiedAmp = min(amplifiedAmp, ADC_MAX/2);
  int micPercent = map(amplifiedAmp, 0, ADC_MAX / 2, 0, 100); // Normalization
  micPercent = constrain(micPercent, 0, 100);
  int barLength = map(micPercent, 0, 100, 0, SCREEN_WIDTH);   // OLED bar
  display.fillRect(0, 24, barLength, 5, SSD1306_WHITE);

  // --- Accelerometer & Gyroscope ---
  mpu.update();
  int isMoving = 0;
  float acc_tot = sqrt(mpu.getAccX()*mpu.getAccX() +        // Total acceleration
                       mpu.getAccY()*mpu.getAccY() +
                       mpu.getAccZ()*mpu.getAccZ());
  float delta_acc = abs(acc_tot - 1.0);                     // Gravity differences
  bool gyro_move = abs(mpu.getGyroX()) > GYRO_THRESHOLD ||  // Gyroscope treshold
                   abs(mpu.getGyroY()) > GYRO_THRESHOLD ||
                   abs(mpu.getGyroZ()) > GYRO_THRESHOLD;
  
  //display.print("Acc X:"); display.print(mpu.getAccX()); display.print(" Y:"); display.print(mpu.getAccY()); display.print(" Z:"); display.println(mpu.getAccZ());
  //display.print("Gyro X:"); display.print(mpu.getGyroX()); display.print(" Y:"); display.print(mpu.getGyroY()); display.print(" Z:"); display.println(mpu.getGyroZ());
  if (delta_acc > ACC_THRESHOLD || gyro_move) {             // Moviment detected
    Serial.print("MOVEMENT! Acc delta: "); Serial.print(delta_acc);
    Serial.print(" GyroX: "); Serial.print(mpu.getGyroX());
    Serial.print(" GyroY: "); Serial.print(mpu.getGyroY());
    Serial.print(" GyroZ: "); Serial.println(mpu.getGyroZ());
    display.println("MOVEMENT DETECTED");
    isMoving = 50;
  }

  // --- Send data to InfluxDB ---
  sensors.clearFields();
  sensors.addField("temperature", t);
  sensors.addField("humidity", h);
  sensors.addField("light", lux);
  sensors.addField("mic", micPercent);
  sensors.addField("accel_x", mpu.getAccX());
  sensors.addField("accel_y", mpu.getAccY());
  sensors.addField("accel_z", mpu.getAccZ());
  sensors.addField("gyro_x", mpu.getGyroX());
  sensors.addField("gyro_y", mpu.getGyroY());
  sensors.addField("gyro_z", mpu.getGyroZ());
  sensors.addField("is_moving", isMoving);
  Serial.println("Sending data to InfluxDB");

  Serial.print("temperature: " + String(t) + " C"); Serial.print(" humidity: " + String(h) + " %"); Serial.println(" light: " + String(lux) + " lx"); 
  Serial.print(" mic: " + String(micPercent)); Serial.println(" Moving: " + String(isMoving));
  //Serial.print(" accel_x: " + String(mpu.getAccX()) + " g"); Serial.print(" accel_y: " + String(mpu.getAccY()) + " g"); Serial.print(" accel_z: " + String(mpu.getAccZ()) + " g");
  //Serial.print(" gyro_x: " + String(mpu.getGyroX()) + " deg/s"); Serial.print(" gyro_y: " + String(mpu.getGyroY()) + " deg/s"); Serial.println(" gyro_z: " + String(mpu.getGyroZ()) + " deg/s");

  if(!client.writePoint(sensors)){
    Serial.print("InfluxDB write failed ");
    Serial.println(client.getLastErrorMessage());
    while(1); // In case of error, stop the program
  }  

  display.display();
  delay(200);
}
