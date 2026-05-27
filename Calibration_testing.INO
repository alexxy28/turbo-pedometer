#include <Arduino.h>

// Accelerometer analog inputs
const int xSig = 4;
const int ySig = 2;
const int zSig = 1;

// High-pass filter tuning.
// Lower cutoff values remove gravity more slowly but preserve motion better.
const float cutoffHz = 0.5f;
const float samplePeriodSeconds = 0.10f;

const float rc = 1.0f / (2.0f * PI * cutoffHz);
const float alpha = rc / (rc + samplePeriodSeconds);

float lastRawX = 0.0f;
float lastRawY = 0.0f;
float lastRawZ = 0.0f;

float filteredX = 0.0f;
float filteredY = 0.0f;
float filteredZ = 0.0f;

bool filterInitialized = false;

int averageAnalogMilliVolts(int pin) {
  long total = 0;
  const int sampleCount = 5;

  for (int i = 0; i < sampleCount; i++) {
    total += analogReadMilliVolts(pin);
    delay(10);
  }

  return total / sampleCount;
}

void applyHighPassFilter(float rawX, float rawY, float rawZ) {
  if (!filterInitialized) {
    lastRawX = rawX;
    lastRawY = rawY;
    lastRawZ = rawZ;
    filterInitialized = true;
    return;
  }

  filteredX = alpha * (filteredX + rawX - lastRawX);
  filteredY = alpha * (filteredY + rawY - lastRawY);
  filteredZ = alpha * (filteredZ + rawZ - lastRawZ);

  lastRawX = rawX;
  lastRawY = rawY;
  lastRawZ = rawZ;
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(xSig, INPUT);
  pinMode(ySig, INPUT);
  pinMode(zSig, INPUT);

  Serial.println("Starting accelerometer high-pass filter test...");
  Serial.println("Keep the sensor still to see gravity attenuate toward zero.");
  Serial.print("Cutoff Hz: ");
  Serial.println(cutoffHz, 2);
  Serial.print("Alpha: ");
  Serial.println(alpha, 4);
}

void loop() {
  float xValue = (float)averageAnalogMilliVolts(xSig);
  float yValue = (float)averageAnalogMilliVolts(ySig);
  float zValue = (float)averageAnalogMilliVolts(zSig);

  applyHighPassFilter(xValue, yValue, zValue);

  Serial.print("Raw mV -> X: ");
  Serial.print(xValue, 2);
  Serial.print(" Y: ");
  Serial.print(yValue, 2);
  Serial.print(" Z: ");
  Serial.println(zValue, 2);

  Serial.print("High-pass mV -> X: ");
  Serial.print(filteredX, 2);
  Serial.print(" Y: ");
  Serial.print(filteredY, 2);
  Serial.print(" Z: ");
  Serial.println(filteredZ, 2);

  float magnitude = sqrt(filteredX * filteredX + filteredY * filteredY + filteredZ * filteredZ);
  Serial.print("Filtered magnitude mV: ");
  Serial.println(magnitude, 2);

  delay(100);
}