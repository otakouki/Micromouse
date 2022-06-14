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
  pinMode(34, INPUT);
  attachInterrupt(digitalPinToInterrupt(34), onRising1, FALLING);
  pinMode(35, INPUT);
  attachInterrupt(digitalPinToInterrupt(35), onRising2, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println("カウンター数:" + String(pulseCounter));
  int a = digitalRead(34);
  Serial.println(a);
  int b = analogRead(34);
  Serial.println(b);
  delay(100);

}
