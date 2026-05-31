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

unsigned long lastMillis = 0;

// Sensor calibration (set these to your sensor's values)
// `sensorOffsetMilliVolts` is the output (mV) corresponding to 0 g (often Vcc/2).
// `sensorSensitivityMilliVoltsPerG` is mV per g (e.g., 330 mV/g for some sensors).
const float sensorOffsetMilliVolts = 1650.0f;
const float sensorSensitivityMilliVoltsPerG = 330.0f;

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

int averageAnalogRaw(int pin) {
  long total = 0;
  const int sampleCount = 5;

  for (int i = 0; i < sampleCount; i++) {
    total += analogRead(pin);
    delay(10);
  }

  return total / sampleCount;
}

void applyHighPassFilter(float rawX, float rawY, float rawZ, float alpha) {
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
  float initialAlpha = rc / (rc + samplePeriodSeconds);
  Serial.print("Alpha (initial estimate): ");
  Serial.println(initialAlpha, 4);
}

void loop() {
  int xRaw = averageAnalogRaw(xSig);
  int yRaw = averageAnalogRaw(ySig);
  int zRaw = averageAnalogRaw(zSig);

  float xValue = (float)averageAnalogMilliVolts(xSig);
  float yValue = (float)averageAnalogMilliVolts(ySig);
  float zValue = (float)averageAnalogMilliVolts(zSig);

  unsigned long now = millis();
  float dtSeconds;
  if (lastMillis == 0) {
    dtSeconds = samplePeriodSeconds; // use estimate for first iteration
  } else {
    dtSeconds = (now - lastMillis) / 1000.0f;
  }
  lastMillis = now;

  float alpha = rc / (rc + dtSeconds);
  if (alpha < 0.0f) alpha = 0.0f;
  else if (alpha > 1.0f) alpha = 1.0f;

  applyHighPassFilter(xValue, yValue, zValue, alpha);

  // Convert raw mV -> g (use offset calibration for raw values)
  float xRawG = (xValue - sensorOffsetMilliVolts) / sensorSensitivityMilliVoltsPerG;
  float yRawG = (yValue - sensorOffsetMilliVolts) / sensorSensitivityMilliVoltsPerG;
  float zRawG = (zValue - sensorOffsetMilliVolts) / sensorSensitivityMilliVoltsPerG;

  // Filtered values are AC (DC removed) so convert directly by sensitivity
  float xFilteredG = filteredX / sensorSensitivityMilliVoltsPerG;
  float yFilteredG = filteredY / sensorSensitivityMilliVoltsPerG;
  float zFilteredG = filteredZ / sensorSensitivityMilliVoltsPerG;

  Serial.print("Raw ADC -> X: ");
  Serial.print(xRaw);
  Serial.print(" Y: ");
  Serial.print(yRaw);
  Serial.print(" Z: ");
  Serial.println(zRaw);

  Serial.print("Raw G -> X: ");
  Serial.print(xRawG, 3);
  Serial.print(" Y: ");
  Serial.print(yRawG, 3);
  Serial.print(" Z: ");
  Serial.println(zRawG, 3);

  Serial.print("High-pass G -> X: ");
  Serial.print(xFilteredG, 3);
  Serial.print(" Y: ");
  Serial.print(yFilteredG, 3);
  Serial.print(" Z: ");
  Serial.println(zFilteredG, 3);

  float magnitudeG = sqrt(xFilteredG * xFilteredG + yFilteredG * yFilteredG + zFilteredG * zFilteredG);
  Serial.print("Filtered magnitude g: ");
  Serial.println(magnitudeG, 3);

  delay(100);
}