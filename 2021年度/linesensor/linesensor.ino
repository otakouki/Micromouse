// MCP3008用サンプルプログラム
//
// CH0とCH1のAD変換値を１秒ごとにシリアル出力
//
//  Arduino          MCP3008
//  SCK(13) <======> CLK
//  MISO(12)<======> DOUT
//  MOSI(11)<======> DIN
//  SS(10)  <======> /CS
//

#include <SPI.h>
#define slaveSelectPin0 5
#define slaveSelectPin1 4

int analogData[16];

int lineSensorR[16] = {0, 0, 0, 0}; //右側に4個ある場合
int lineSensorL[16] = {0, 0, 0, 0}; //左側に4個ある場合

void setup()
{
  Serial.begin(115200);             //シリアル通信開始
  pinMode(slaveSelectPin0, OUTPUT); // SS(10)を出力に設定
  pinMode(slaveSelectPin1, OUTPUT);
  SPI.setBitOrder(MSBFIRST);           // MSBから送信
  SPI.setClockDivider(SPI_CLOCK_DIV2); //クロック分周設定
  SPI.setDataMode(SPI_MODE0);          // SPIモード0
  SPI.begin();                         // SPI通信開始
}

void loop()
{
  byte data[4] = {0, 0, 0, 0};
  // 引数にチャンネルを設定
  get_adc(0);
  Serial.print("ch1:L");
  Serial.println(analogData[0]);
  Serial.print("ch1:R");
  Serial.println(analogData[1]);
  lineSensorR[0] = analogData[1];
  delay(1000);
  get_adc(1);
  Serial.print("ch2:L");
  Serial.println(analogData[0]);
  Serial.print("ch2:R");
  Serial.println(analogData[1]);
  lineSensorR[1] = analogData[1];
  delay(1000);
  get_adc(2);
  Serial.print("ch3:L");
  Serial.println(analogData[0]);
  Serial.print("ch3:R");
  Serial.println(analogData[1]);
  lineSensorR[2] = analogData[1];
  delay(1000);
  get_adc(3);
  Serial.print("ch4:L");
  Serial.println(analogData[0]);
  Serial.print("ch4:R");
  Serial.println(analogData[1]);
  //  for (int i = 0; i <= 3; i++) {
  //    //    //------[ ADC0のデータを取得する ]------
  //    //    digitalWrite(slaveSelectPin0, LOW);       //CS LOW
  //    //    SPI.transfer(0x01);                       //①スタートビット送信
  //    //    lineSensorR[i] = ((SPI.transfer((i << 4) | 0x80) & 0x03) << 8) | SPI.transfer(0);  //ADC0
  //    //    digitalWrite(slaveSelectPin0, HIGH);      //CS HIGH
  //    //
  //    //    Serial.println(lineSensorR[0]);
  //    //------[ ADC0のデータを取得する ]------
  //
  //    digitalWrite(slaveSelectPin0, LOW);       //CS LOW
  //    SPI.transfer(0x01);                       //①スタートビット送信
  //    data[0] = SPI.transfer((i << 4) | 0x80); //②Single-ended チャンネル選択,ADC0のbit9,8取得
  //    data[1] = SPI.transfer(0);                //③ADC0のbit7～0取得
  //
  //    digitalWrite(slaveSelectPin0, HIGH);      //CS HIGH
  //  }
  delay(1000);
}

void get_adc(byte ch)
{
  byte data[4] = {0, 0, 0, 0}; // SPI通信用変数

  //------[ ADC0のデータを取得する ]------
  digitalWrite(slaveSelectPin0, LOW);       // CS LOW
  SPI.transfer(0x01);                       //①スタートビット送信
  data[0] = SPI.transfer((ch << 4) | 0x80); //②Single-ended チャンネル選択,ADC0のbit9,8取得
  Serial.println(data[0]);
  data[1] = SPI.transfer(0); //③ADC0のbit7～0取得
  Serial.println(data[1]);
  digitalWrite(slaveSelectPin0, HIGH); // CS HIGH

  // ------[ ADC1のデータを取得する ]------
  digitalWrite(slaveSelectPin1, LOW);       // CS LOW
  SPI.transfer(0x01);                       //①スタートビット送信
  data[2] = SPI.transfer((ch << 4) | 0x80); //②Single-ended チャンネル選択,ADC0のbit9,8取得
  data[3] = SPI.transfer(0);                //③ADC0のbit7～0取得
  digitalWrite(slaveSelectPin1, HIGH);      // CS HIGH

  analogData[0] = ((data[0] & 0x03) << 8) | data[1]; // ADC0
  analogData[1] = ((data[2] & 0x03) << 8) | data[3]; // ADC1
  // return analogData[0];
}
