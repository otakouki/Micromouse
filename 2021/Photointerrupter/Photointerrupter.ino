#include "driver/pcnt.h"

int16_t count = 0; //カウント数
unsigned long pulseCounter = 0;
unsigned long pulseCounter2 = 0;
int a = digitalRead(34);
int b = digitalRead(35);
boolean flg = false;
boolean flg2 = false;
void onRising1() {
  if (a == 0) {
    if (flag == false) {
      ++pulseCounter;
    }
    flg = true;
  }else{
    flg = false;
  }
}

void onRising2() {
  if (b == 0) {
    if (flg2 == false) {
      ++pulseCounter2;
    }
    flg2 = true;
  }else{
    flg2 = false;
  }
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
  a = digitalRead(34);
  Serial.println(a);
  b = digitalRead(34);
  Serial.println(b);
  delay(100);

}
