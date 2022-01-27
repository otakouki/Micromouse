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
  pinMode(16, INPUT);
  attachInterrupt(digitalPinToInterrupt(16), onRising1, RISING);
  pinMode(17, INPUT);
  attachInterrupt(digitalPinToInterrupt(17), onRising2, RISING);


}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println(String(pulseCounter));
  delay(5000);

}
