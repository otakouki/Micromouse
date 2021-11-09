#include "driver/pcnt.h"

int16_t count = 0; //カウント数
unsigned long pulseCounter = 0;
unsigned long pulseCounter2 = 0;

void onRising1() {
  ++pulseCounter;
}
void onRising2() {
  ++pulseCounter2;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(27, INPUT);
  attachInterrupt(digitalPinToInterrupt(27), onRising1, RISING);
  pinMode(13, INPUT);
  attachInterrupt(digitalPinToInterrupt(13), onRising2, RISING);


}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("pulseCounter1: " + String(pulseCounter));
  Serial.println("pulseCounter2: " + String(pulseCounter2));

}
