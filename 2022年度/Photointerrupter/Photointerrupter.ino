#include "driver/pcnt.h"

int16_t count = 0;               // カウント数
unsigned long pulseCounter = 0;  // 割り込み処理
unsigned long pulseCounter2 = 0; // 割り込み処理

// 割り込み処理(34番PIN)
void onRising1()
{
  ++pulseCounter;
}
// 割り込み処理(35番PIN)
void onRising2()
{
  ++pulseCounter2;
}

void setup()
{
  Serial.begin(115200);
  // PIN番号の設定
  pinMode(36, INPUT);
  pinMode(39, INPUT);
}

void loop()
{
  // 割り込み処理としてonRisin1を呼ぶ
  attachInterrupt(digitalPinToInterrupt(36), onRising1, FALLING);
  // 割り込み処理としてonRisin2を呼ぶ
  attachInterrupt(digitalPinToInterrupt(39), onRising2, FALLING);
  Serial.println("カウンター数:" + String(pulseCounter));
  Serial.println("カウンター数2:" + String(pulseCounter2));

  delay(100);
}
