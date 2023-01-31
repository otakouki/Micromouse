/**
 * @file Micromouse_Production.ino
 * @author kokiota
 * @brief マイクロマウス本番環境コード
 * @version 0.1
 * @date 2023-01-17
 *
 * @copyright Copyright (c) 2023
 *
 */

// モーター回転数
#define MIN_PWM 5
#define MIN_HALF_PWM 10
#define HALF_PWM 15
#define MAX_HALF_PWM 20
#define MAX_PWM 25

// ラインセンサーの初期化開始
#include <SPI.h>
#define slaveSelectPin0 5
#define slaveSelectPin1 13
#define TH_LVL 250
int analogData[16];
int R_sns[6];
int R_sns_old[6];
int L_sns[6];
int L_sns_old[6];

// モータードライバの初期化
const int CHANNEL_A = 0;
const int CHANNEL_B = 1;

const int LEDC_TIMER_BIT = 8;
const int LEDC_BASE_FREQ = 490;

const int motorA[3] = {32, 33, 14}; // AIN1, AIN2, PWMA
const int motorB[3] = {16, 17, 12}; // BIN1, BIN2, PWMB

bool corner_status = false;
bool sg_status = false;
// LED pin
#define LED 25

// フォトインタラプタの初期化
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
  Serial.begin(115200);             // シリアル通信開始
  pinMode(slaveSelectPin0, OUTPUT); // SS(10)を出力に設定
  pinMode(slaveSelectPin1, OUTPUT);
  SPI.setBitOrder(MSBFIRST);           // MSBから送信
  SPI.setClockDivider(SPI_CLOCK_DIV2); // クロック分周設定
  SPI.setDataMode(SPI_MODE0);          // SPIモード0
  SPI.begin();                         // SPI通信開始

  // LEDピンの出力設定
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  Serial.println("HIGH");

  // フォトインタラプタの設定
  pinMode(36, INPUT);
  pinMode(39, INPUT);

  // モータードライバセットアップ
  for (int i = 0; i < 3; i++)
  {
    pinMode(motorA[i], OUTPUT);
    pinMode(motorB[i], OUTPUT);
  }

  ledcSetup(CHANNEL_A, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcSetup(CHANNEL_B, LEDC_BASE_FREQ, LEDC_TIMER_BIT);

  ledcAttachPin(motorA[2], CHANNEL_A);
  ledcAttachPin(motorB[2], CHANNEL_B);
}

void loop()
{
  static bool mortar_status = false;
  get_adc();
  if (mortar_status == true)
  {
    if (corner_status == false)
    {
      if (R_sns[0] == 1)
      {
        // Serial.printf("モーターを止める\n");
        mortar_status = false;
        mortar_stop();
        // 割り込み処理の停止
        detachInterrupt(digitalPinToInterrupt(36));
        Serial.printf("pulseCounter:stop\n");

        // 割り込み処理の停止
        detachInterrupt(digitalPinToInterrupt(39));
        Serial.printf("pulseCounter2:stop\n");
        delay(2000);
      }
      else
      {
        if (L_sns[1] == 1)
        {
          mortar(MIN_PWM, MAX_PWM);
        }
        else if (L_sns[2] == 1)
        {
          mortar(MIN_HALF_PWM, MAX_PWM);
        }
        else if (L_sns[3] == 1)
        {
          mortar(HALF_PWM, MAX_PWM);
        }
        else if (L_sns[4] == 1)
        {
          mortar(MAX_HALF_PWM, MAX_PWM);
        }
        else if (L_sns[5] == 1)
        {
          mortar(MAX_PWM, MAX_PWM);
        }
        else if (R_sns[1] == 1)
        {
          mortar(MAX_PWM, MIN_PWM);
        }
        else if (R_sns[2] == 1)
        {
          mortar(MAX_PWM, MIN_HALF_PWM);
        }
        else if (R_sns[3] == 1)
        {
          mortar(MAX_PWM, HALF_PWM);
        }
        else if (R_sns[4] == 1)
        {
          mortar(MAX_PWM, MAX_HALF_PWM);
        }
        else if (R_sns[5] == 1)
        {
          mortar(MAX_PWM, MAX_PWM);
        }
      }
    }
    else
    {
    }
    // Serial.printf("カウンター数:%d\n", pulseCounter);
    // Serial.printf("カウンター数2:%d\n" + pulseCounter2);
  }
  else
  {
    if (R_sns[0] == 1)
    {
      // Serial.printf("モーターを回す\n");
      mortar_status = true;
      for (int i = 0; i < 25; i++)
      {
        mortar(i, i);
      }
      // 割り込み処理としてonRisin1を呼ぶ
      attachInterrupt(digitalPinToInterrupt(36), onRising1, FALLING);
      // 割り込み処理としてonRisin2を呼ぶ
      attachInterrupt(digitalPinToInterrupt(39), onRising2, FALLING);
      delay(1000);
    }
  }
}
void get_adc()
{
  for (int ch = 0; ch < 6; ch++)
  {
    byte data[4] = {0, 0, 0, 0}; // SPI通信用変数
    //------[ ADC0のデータを取得する ]------
    digitalWrite(slaveSelectPin0, LOW);       // CS LOW
    SPI.transfer(0x01);                       // ①スタートビット送信
    data[0] = SPI.transfer((ch << 4) | 0x80); // ②Single-ended チャンネル選択,ADC0のbit9,8取得
    data[1] = SPI.transfer(0);                // ③ADC0のbit7～0取得
    digitalWrite(slaveSelectPin0, HIGH);      // CS HIGH

    // ------[ ADC1のデータを取得する ]------
    digitalWrite(slaveSelectPin1, LOW);       // CS LOW
    SPI.transfer(0x01);                       // ①スタートビット送信
    data[2] = SPI.transfer((ch << 4) | 0x80); // ②Single-ended チャンネル選択,ADC0のbit9,8取得
    data[3] = SPI.transfer(0);                // ③ADC0のbit7～0取得
    digitalWrite(slaveSelectPin1, HIGH);      // CS HIGH

    analogData[0] = ((data[0] & 0x03) << 8) | data[1]; // ADC0
    analogData[1] = ((data[2] & 0x03) << 8) | data[3]; // ADC1

    if ((int)analogData[0] <= TH_LVL)
    {
      /* code */
      R_sns[ch] = (int)1;
    }
    else if ((int)analogData[0] > TH_LVL)
    {
      if (ch == 0 && (int)analogData[0] <= 300)
      {
        R_sns[ch] = (int)1;
      }
      R_sns[ch] = (int)0;
    }

    if ((int)analogData[1] <= TH_LVL)
    {
      /* code */
      L_sns[ch] = (int)1;
    }
    else if ((int)analogData[1] > TH_LVL)
    {
      L_sns[ch] = (int)0;
    }
  }
}

void mortar(int PWM_A, int PWM_B)
{
  Serial.printf("PWM_A:\t%d\n", PWM_A);
  Serial.printf("PWM_B:\t%d\n", PWM_B);
  digitalWrite(motorA[1], HIGH);
  digitalWrite(motorA[0], LOW);
  ledcWrite(CHANNEL_A, PWM_A);

  // 右モータ（CW，時計回り）
  digitalWrite(motorB[1], LOW);
  digitalWrite(motorB[0], HIGH);
  ledcWrite(CHANNEL_B, PWM_B);
}

void mortar_stop()
{
  digitalWrite(motorA[0], HIGH);
  digitalWrite(motorA[1], HIGH);
  digitalWrite(motorB[0], HIGH);
  digitalWrite(motorB[1], HIGH);
  Serial.printf("モーター止まりました");
}

void sensor_off() {}
void sensor_on() {}
