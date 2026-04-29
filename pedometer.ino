/*
  pedometer.ino
  ESP32-S2 Mini starter sketch
*/

const int xSig = 5;
const int ySig = 3;
const int zSig = 1;

void setup() {
  

  // Analog signal inputs
  pinMode(xSig, INPUT);
  pinMode(ySig, INPUT);
  pinMode(zSig, INPUT);
}

void loop() {

}
