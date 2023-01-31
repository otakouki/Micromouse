/**
 * @file MCP3008.ino
 * @author otakoki
 * @brief マイクロマウスのラインセンサの値取得コード
 * @version 0.1
 * @date 2022-12-21
 *
 * @copyright Copyright (c) 2022
 *
 */
// SPIを使うため宣言
#include <SPI.h>
// cs1の設定
#define slaveSelectPin0 5
// cs2の設定
#define slaveSelectPin1 13
// 白か黒の判定するための基準値
#define TH_LVL 250
// cs1,2のチャンネルの値をアナログ値で格納
int analogData[16];
// cs1の値をデジタルで格納
int R_sns[6];
// cs1の前のデータを格納
int R_sns_old[6];
// cs2の値をデジタルで格納
int L_sns[6];
// cs2の前のデータを格納
int L_sns_old[6];

void setup()
{
  Serial.begin(115200);                // シリアル通信開始
  pinMode(slaveSelectPin0, OUTPUT);    // SS(5)を出力に設定
  pinMode(slaveSelectPin1, OUTPUT);    // ss(13)を出力に設定
  SPI.setBitOrder(MSBFIRST);           // 最上位ビットから取得
  SPI.setClockDivider(SPI_CLOCK_DIV2); // クロック分周設定
  SPI.setDataMode(SPI_MODE0);          // SPI_MODE0(アイドル時のクロックがLow、立ち上がりでサンプリング)
  SPI.begin();                         // SPI通信開始
}

void loop()
{
  // ADコンバータの値呼び出し関数
  get_adc();
  // cs1の値を表示
  Serial.print("R_sns:[");
  for (int r = 0; r < 6; r++)
  {
    Serial.printf("%d , ", R_sns[r]);
  }
  Serial.println("]");
  // cs2の値を表示
  Serial.print("L_sns:[");
  for (int l = 0; l < 6; l++)
  {
    Serial.printf("%d , ", L_sns[l]);
  }
  Serial.println("]");
  delay(1);
}
/**
 * @brief ADコンバータの値取得
 */
void get_adc()
{
  // ADコンバータの使用しているチャンネル数繰り返し
  for (int ch = 0; ch < 6; ch++)
  {
    byte data[4] = {0, 0, 0, 0}; // SPI通信用変数
    //------[ ADC0のデータを取得する ]------
    digitalWrite(slaveSelectPin0, LOW);       // CS1をLOWに設定
    SPI.transfer(0x01);                       // ①スタートビット送信
    data[0] = SPI.transfer((ch << 4) | 0x80); // ②Single-ended チャンネル選択,ADC0のbit9,8取得
    data[1] = SPI.transfer(0);                // ③ADC0のbit7～0取得
    digitalWrite(slaveSelectPin0, HIGH);      // CS1 HIGH

    // ------[ ADC1のデータを取得する ]------
    digitalWrite(slaveSelectPin1, LOW);       // CS2 LOW
    SPI.transfer(0x01);                       // ①スタートビット送信
    data[2] = SPI.transfer((ch << 4) | 0x80); // ②Single-ended チャンネル選択,ADC0のbit9,8取得
    data[3] = SPI.transfer(0);                // ③ADC1のbit7～0取得
    digitalWrite(slaveSelectPin1, HIGH);      // CS2 HIGH

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
