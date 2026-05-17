/*
  pedometer.ino
  ESP32-S2 Mini starter sketch
*/
// FreeRTOS includes for threading


//Pins for each axis of the accelerometer
const int xSig = 4;
const int ySig = 2;
const int zSig = 1;

//pin for the SelfTest Enable
const int ST = 6;
const int thresholdX = 50;
const int thresholdY = 50;
const int thresholdZ = 50;
//Self Test results
bool selfTestPassed = false;

int averageAnalogMilliVolts(int pin) {
  long total = 0;
  const int sampleCount = 5;

  for (int i = 0; i < sampleCount; i++) {
    total += analogReadMilliVolts(pin);
    delay(10);
  }

  return total / sampleCount;
}

bool selftest() {
  int selfTestAttempts = 0;
  while (selfTestAttempts < 3 && !selfTestPassed) {
    // Log attempt
    Serial.print("Self-test attempt ");
    Serial.println(selfTestAttempts + 1);

    // Enable self test mode
    digitalWrite(ST, LOW);
    delay(100); // Wait for the self test to stabilize

    // Read the accelerometer values in self test mode
    int xSelfTest = averageAnalogMilliVolts(xSig);
    int ySelfTest = averageAnalogMilliVolts(ySig);
    int zSelfTest = averageAnalogMilliVolts(zSig);

    Serial.print("SelfTest readings mV: X="); Serial.print(xSelfTest);
    Serial.print(" Y="); Serial.print(ySelfTest);
    Serial.print(" Z="); Serial.println(zSelfTest);

    // Disable self test mode
    digitalWrite(ST, HIGH);
    delay(100);

    // Read the accelerometer values in normal mode
    int xNormal = averageAnalogMilliVolts(xSig);
    int yNormal = averageAnalogMilliVolts(ySig);
    int zNormal = averageAnalogMilliVolts(zSig);

    Serial.print("Normal readings mV: X="); Serial.print(xNormal);
    Serial.print(" Y="); Serial.print(yNormal);
    Serial.print(" Z="); Serial.println(zNormal);

    // Calculate the signed deltas in millivolts
    int xDiff = xSelfTest - xNormal;
    int yDiff = ySelfTest - yNormal;
    int zDiff = zSelfTest - zNormal;

    Serial.print("Diff mV: X="); Serial.print(xDiff);
    Serial.print(" Y="); Serial.print(yDiff);
    Serial.print(" Z="); Serial.println(zDiff);

    // Check if the deltas are within the expected ranges
    if (xDiff > -800 && xDiff < -430 &&
        yDiff > 200 && yDiff < 430 &&
        zDiff > 200 && zDiff < 730) {
      selfTestPassed = true;
      Serial.println("Self-test: PASS");
      return true; // Self test passed
    }

    Serial.println("Self-test attempt result: FAIL");

    ++selfTestAttempts; // Increment the self test attempts counter
  }

  return false; // Self test failed
}

void setup() {
  // Initialize Serial for debug output
  Serial.begin(115200);
  delay(100);
  Serial.println("Starting pedometer self-test...");

  // PinMode for ADXL inputs and Self Test output
  // Analog signal inputs
  pinMode(xSig, INPUT);
  pinMode(ySig, INPUT);
  pinMode(zSig, INPUT);
  //output pin for Self Test
  pinMode(ST, OUTPUT);

  // Run the self-test once at startup and report result
  bool passed = selftest();
  if (passed) {
    Serial.println("Self-test PASSED");
  } else {
    Serial.println("Self-test FAILED");
  }
}


void loop() {




}
