#include <Arduino.h>
#include <Wire.h>

#define BUZZER_PIN 25
#define MPU6050_ADDR 0x68

#define PWR_MGMT_1   0x6B
#define ACCEL_XOUT_H 0x3B

const float EARTHQUAKE_THRESHOLD = 3.0;

float accelMagnitude;
unsigned long lastAlarmTime = 0;
const unsigned long ALARM_DURATION = 5000;
bool isAlarming = false;
float baselineAccel = 9.81;
const int CALIBRATION_SAMPLES = 100;

void calibrateSensor();
void readMPU6050(int16_t &ax, int16_t &ay, int16_t &az);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  

  Serial.println("Testing buzzer...");
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(150);
    digitalWrite(BUZZER_PIN, LOW);
    delay(150);
  }
  
  
  Wire.begin(21, 22); 
  delay(500);
  
  Serial.println("Initializing MPU6050...");
  

  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(PWR_MGMT_1);
  Wire.write(0); 
  byte error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.println("MPU6050 connected successfully!");
  } else {
    Serial.println("MPU6050 connection failed!");
    Serial.print("Error code: ");
    Serial.println(error);
    while (1) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);
      digitalWrite(BUZZER_PIN, LOW);
      delay(100);
    }
  }
  
  delay(100);
  
  calibrateSensor();
  
}

void readMPU6050(int16_t &ax, int16_t &ay, int16_t &az) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(ACCEL_XOUT_H);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 6, true);
  
  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();
}

void calibrateSensor() {
  Serial.println("Calibrating sensor...");
  Serial.println("JANGAN GERAKKAN SENSOR!");
  
  float sumMagnitude = 0;
  int16_t ax, ay, az;
  
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    readMPU6050(ax, ay, az);
    

    float axms = (ax / 16384.0) * 9.81;
    float ayms = (ay / 16384.0) * 9.81;
    float azms = (az / 16384.0) * 9.81;
    
    float magnitude = sqrt(axms*axms + ayms*ayms + azms*azms);
    sumMagnitude += magnitude;
    
    if (i % 20 == 0) {
      Serial.print(".");
    }
    
    delay(10);
  }
  Serial.println();
  
  baselineAccel = sumMagnitude / CALIBRATION_SAMPLES;
  Serial.print("Baseline acceleration: ");
  Serial.print(baselineAccel);
  Serial.println(" m/s²");
  Serial.println("Calibration complete!");
}

void loop() {
  int16_t ax, ay, az;
  

  readMPU6050(ax, ay, az);
  

  float axms = (ax / 16384.0) * 9.81;
  float ayms = (ay / 16384.0) * 9.81;
  float azms = (az / 16384.0) * 9.81;
  

  accelMagnitude = sqrt(axms*axms + ayms*ayms + azms*azms);
  

  float deviation = abs(accelMagnitude - baselineAccel);
  

  if (deviation > EARTHQUAKE_THRESHOLD) {
    if (!isAlarming) {
      Serial.println("\n========================");
      Serial.println("!!! GEMPA TERDETEKSI !!!");
      Serial.println("========================");
      Serial.print("Magnitude: ");
      Serial.print(accelMagnitude, 2);
      Serial.print(" m/s² | Deviation: ");
      Serial.print(deviation, 2);
      Serial.println(" m/s²");
      Serial.print("X: ");
      Serial.print(axms, 2);
      Serial.print(" Y: ");
      Serial.print(ayms, 2);
      Serial.print(" Z: ");
      Serial.println(azms, 2);
      
      isAlarming = true;
      lastAlarmTime = millis();
    }
  }
  

  if (isAlarming) {
    unsigned long currentTime = millis();
    if ((currentTime - lastAlarmTime) % 400 < 200) {
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }
    
    if (currentTime - lastAlarmTime > ALARM_DURATION) {
      isAlarming = false;
      digitalWrite(BUZZER_PIN, LOW);
      Serial.println("=== Alarm stopped ===\n");
    }
  }
  

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    Serial.print("Accel: ");
    Serial.print(accelMagnitude, 2);
    Serial.print(" m/s² | Dev: ");
    Serial.print(deviation, 2);
    Serial.print(" m/s² | ");
    
    if (isAlarming) {
      Serial.print("[ALARM] ");
    } else if (deviation > EARTHQUAKE_THRESHOLD * 0.5) {
      Serial.print("[WARN]  ");
    } else {
      Serial.print("[OK]    ");
    }
    
    int bars = (int)(deviation / EARTHQUAKE_THRESHOLD * 20);
    Serial.print("[");
    for (int i = 0; i < 20; i++) {
      if (i < bars) Serial.print("#");
      else Serial.print("-");
    }
    Serial.println("]"); 
    
    lastPrint = millis();
  }
  
  delay(50);
}