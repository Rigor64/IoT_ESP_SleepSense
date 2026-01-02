/*
 *      SleepSense - Smart sleep monitoring system
 *
 *   SSD1306  Vcc    --- 3.3V
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
 *            out    --- 34
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
#define WIFI_SSID       "PosteMobile-79159793_EXT"
#define WIFI_PASSWORD   "4PF9yedTc2FdN5kzfu4EuSQk"
#define INFLUXDB_URL    "http://informatica-iot.freeddns.org:8086/"
#define INFLUXDB_ORG    "uniurb"
#define INFLUXDB_BUCKET "esercitazioni"
#define INFLUXDB_TOKEN  "qt5kHYb2Wwg19lbGEOe3BmVJhlMf6ZxfDu_Z7Lrhiiiv9FTEGKiD_pJzkDa_qSlUPLPm0zI-1yE6THb4kg-0kA=="
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN,
                      InfluxDbCloud2CACert); // InfluxDB instance
Point sensors("corso_IoT"); // Data point

// === NTP ===
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;   // +1h for Italy
const int   daylightOffset_sec = 3600;  // +1h for summer time

// === OLED display ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define MAX_LINES 8
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// === Humidity & Temperature sensor ===
#define DHTPIN 14    // Connect DHT11 to GPIO14 pin
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// === Light sensor ===
BH1750 lightMeter;

// === Microphone ===
#define MIC_PIN 34   // Connect MAX4466 to GPIO34 pin
#define SAMPLES 128
#define ADC_MAX 4095 // ESP32 ADC max value
#define NOISE_THRESHOLD 50
int MID_VALUE = 2048;
float smoothAmp = 0;
const float alpha = 0.25;

// === Accelerometer & Gyroscope ===
MPU6050 mpu(Wire); 
const float ACC_THRESHOLD = 0.10;   // Minimum variation of acceleration (g) for moviment detection
const float GYRO_THRESHOLD = 3.0;  // Minimum variation of gyroscope (deg/s) for moviment detection

void setup() {
  Wire.begin(21, 22); // I2C pins (21 = SDA, 22 = SCL)
  Serial.begin(115200); // Serial console
  analogReadResolution(12); // ESP32 ADC on 12 bit (0-4095)

  // Configure the watchdog to reboot the CPU if not reset within 20 seconds
  esp_task_wdt_config_t config = {
    .timeout_ms = 20* 1000,    //  20 seconds
    .trigger_panic = true,     // Trigger panic == reset CPU (reboot)
  };
  esp_task_wdt_reconfigure(&config); // Configure WDT
  enableLoopWDT(); // Enable WDT in loop()

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED error")); 
    for(;;);
  }
  
  // First display
  display.display();  
  display.clearDisplay();
  display.setCursor(0,0);  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  Serial.println("OLED initialized");
  display.println("OLED ready");

  // --- WiFi connection ---
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { // Wait for connection
    delay(500);
    Serial.print(".");
    display.print(".");
  }
  Serial.println("");

  delay(2000);

  // Second display
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  Serial.print("RSSI: "); Serial.println(WiFi.RSSI());
  display.println("");
  display.println("WiFi connected");
  display.print("IP: "); display.println(WiFi.localIP());
  display.print("RSSI: "); display.println(WiFi.RSSI());

  delay(2000);

  display.display();
  display.clearDisplay();
  display.setCursor(0,0);

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
  while(status!=0){ 
    Serial.println("MPU6050 not responding...");
    delay(1000);   // permette al WDT di non scattare
  }
  mpu.calcOffsets(true,true);
  Serial.println("MPU6050 ready");
  display.println("MPU6050 ready");
  
  // --- Microphone ---  
  display.println("MAX4466 ready");

  display.display();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  display.clearDisplay();
  display.setCursor(0,0);

  // --- Timestamp  
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  char timeStr[20];
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);
  display.println(timeStr);

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
  for (int i = 0; i < SAMPLES; i++) {                       // Calculate the mean of multiple sampling
    int micValue = analogRead(MIC_PIN);
    int amplitude = abs(micValue - MID_VALUE);              // Sound amplification
    MID_VALUE = 0.99 * MID_VALUE + 0.01 * micValue;             
    if (amplitude < NOISE_THRESHOLD) amplitude = 0;         // Ignora piccoli rumori             
    smoothAmp = alpha * amplitude + (1 - alpha) * smoothAmp;
    delayMicroseconds(50);                                  // Sampling rapido
    yield();
  }
  int amplifiedAmp = smoothAmp * 3;                           // Software amplification
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
  
  display.println(" ");
  //display.print("Acc X:"); display.print(mpu.getAccX()); display.print(" Y:"); display.print(mpu.getAccY()); display.print(" Z:"); display.println(mpu.getAccZ());
  //display.print("Gyro X:"); display.print(mpu.getGyroX()); display.print(" Y:"); display.print(mpu.getGyroY()); display.print(" Z:"); display.println(mpu.getGyroZ());
  if (delta_acc > ACC_THRESHOLD || gyro_move) {             // Moviment detected
    //display.println("MOVEMENT DETECTED");
    isMoving = 50;
    display.fillCircle(120, 12, 5, SSD1306_WHITE);
    Serial.print("MOVEMENT! Acc delta: "); Serial.print(delta_acc);
  }else {
    display.fillCircle(120, 12, 5, SSD1306_BLACK);
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

  Serial.print("temperature: " + String(t) + " C"); Serial.print(" humidity: " + String(h) + " %"); Serial.print(" light: " + String(lux) + " lx");  Serial.print(" mic: " + String(micPercent)); Serial.println(" Moving: " + String(isMoving));
  //Serial.print(" accel_x: " + String(mpu.getAccX()) + " g"); Serial.print(" accel_y: " + String(mpu.getAccY()) + " g"); Serial.print(" accel_z: " + String(mpu.getAccZ()) + " g");
  //Serial.print(" gyro_x: " + String(mpu.getGyroX()) + " deg/s"); Serial.print(" gyro_y: " + String(mpu.getGyroY()) + " deg/s"); Serial.println(" gyro_z: " + String(mpu.getGyroZ()) + " deg/s");

  if(!client.writePoint(sensors)){
    Serial.print("InfluxDB write failed ");
    Serial.println(client.getLastErrorMessage());
  }  

  display.display();
  delay(200);
}
