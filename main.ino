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
const int thresholdX = 100;
const int thresholdY = 100;
const int thresholdZ = 100;
//Self Test results
bool selfTestPassed = false;


// Pace Detection Variables
int paces = 0;
bool testing = true;

const int led1 = 13;
const int led2 = 12;
const int led3 = 14;

int currentSteps = 0;
int lastSteps = 0;

unsigned long paceTimer = 0;
// End Pace Detection Var


// Button Input
const int button = 10;
const int debounceDelay = 50; // milliseconds
unsigned long lastButtonPressTime = 0;
// End Button Input


int averageAnalogMilliVolts(int pin) {
  long total = 0;
  const int sampleCount = 5;

  for (int i = 0; i < sampleCount; i++) {
    total += analogReadMilliVolts(pin);
    delay(10);
  }

  return total / sampleCount;
}

bool buttonPressed() {
  unsigned long currentTime = millis();
  if (digitalRead(button) == LOW && (currentTime - lastButtonPressTime) > debounceDelay) {
    lastButtonPressTime = currentTime;
    return true;
  }
  return false;
}

void paceDetect() {
  switch (paces) {
    if(millis() - paceTimer > 3000) {
    paceTimer = millis();
    case 0:
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);

      if(currentSteps - lastSteps >= 1) {
        paces = 1;
      }
      
      break;
    case 1:
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);

        if(currentSteps - lastSteps >= 3) {
          paces = 2;
        } else if (currentSteps - lastSteps < 1) {
          paces = 0;
        }
      break;
    case 2: 
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, LOW);
        if(currentSteps - lastSteps >= 5) {
          paces = 3;
        } else if (currentSteps - lastSteps < 3) {
          paces = 1;
        }
      break;
    case 3:
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      if(currentSteps - lastSteps < 5) {
        paces = 2;
      }
      break;
  }
  lastSteps = currentSteps;
  if(testing) {
    currentSteps += random(0, 7);
  }
}
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

  // Outputs for Pace Detection
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  // Button Input
  pinMode(button, INPUT_PULLUP);


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
