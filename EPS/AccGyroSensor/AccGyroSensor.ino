#include <Wire.h>
#include <MPU6050_light.h>
#include <Adafruit_GFX.h>     // Core graphics
#include <Adafruit_SSD1306.h> // OLED display

MPU6050 mpu(Wire);

// === OLED display ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const float ACC_THRESHOLD = 0.05;   // variazione minima di accelerazione (g) per movimento
const float GYRO_THRESHOLD = 20.0; 

unsigned long timer = 0;

void setup() {
  Wire.begin(21, 22);
  Serial.begin(115200);

  // Inizializza MPU6050
  byte status = mpu.begin();
  if (status != 0) {
    Serial.println("MPU non trovato!");
    while(1);
  }
  Serial.print(F("MPU6050 status: ")); Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(true,true); // gyro and accelero

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
  display.println("MPU6050 ready");


  Serial.println("Done!\n");
  delay(1000);
}

void loop() {
  mpu.update();
  display.clearDisplay();
  display.setCursor(0,0);

  float acc_tot = sqrt(mpu.getAccX()*mpu.getAccX() +
                       mpu.getAccY()*mpu.getAccY() +
                       mpu.getAccZ()*mpu.getAccZ());

  // Differenza dalla gravità 1g
  float delta_acc = abs(acc_tot - 1.0);

  // Controllo soglia giroscopio
  bool gyro_move = abs(mpu.getGyroX()) > GYRO_THRESHOLD ||
                   abs(mpu.getGyroY()) > GYRO_THRESHOLD ||
                   abs(mpu.getGyroZ()) > GYRO_THRESHOLD;

  // Movimento rilevato
  if (delta_acc > ACC_THRESHOLD || gyro_move) {
    Serial.print("MOVIMENTO! Acc delta: "); Serial.print(delta_acc);
    Serial.print(" GyroX: "); Serial.print(mpu.getGyroX());
    Serial.print(" GyroY: "); Serial.print(mpu.getGyroY());
    Serial.print(" GyroZ: "); Serial.println(mpu.getGyroZ());
  }

  // Stampare sempre per logging
  display.print("ACC_X: "); display.print(mpu.getAccX());
  display.print(" Y: "); display.print(mpu.getAccY());
  display.print(" Z: "); display.print(mpu.getAccZ());
  display.print(" | ACC_tot: "); display.print(acc_tot);
  display.print(" | GYRO_X: "); display.print(mpu.getGyroX());
  display.print(" Y: "); display.print(mpu.getGyroY());
  display.print(" Z: "); display.println(mpu.getGyroZ());
  display.display();
  delay(50);

}