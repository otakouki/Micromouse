#include <SPI.h>
#include <Wire.h>

// I2Cアドレス
#define DRV_ADR_R 0x64 // RightDRV8830のI2Cアドレス
#define DRV_ADR_L 0x60 // LeftDRV8830のI2Cアドレス
#define CTR_ADR 0x00 // CONTROLレジスタのサブアドレス
#define FLT_ADR 0x01 // FAULTレジスタのアドレス

// ブリッジ制御
#define STOP      0x00  //停止
#define FORWARD   0x01  //正転
#define BACKWARD  0x02  //逆転
#define BREAK  0x03  //ブレーキ

// 電圧定義
#define MAX_VSET 0x23 // 5.06V
#define MID_VSET 0x30 // 3.86V
#define MIN_VSET 0x09 // 0.72V

#define slaveSelectPin0 5
#define slaveSelectPin1 4

float Kp = 0.2; //仮

int analogData[16];

int lineSensorR[16] = {0, 0, 0}; //右側に4個ある場合
int lineSensorL[16] = {0, 0, 0}; //左側に4個ある場合
int len = 3;
//int std = 100;


/*
  処理：ラインが何番目のフォトリフレクタの下にあるか判定
  引数：int ary[]: ラインセンサーのアナログ値が入った配列
  戻値：ラインセンサーの位置
*/

int getRsensor(int ary[])
{
  //ラインが何番目のフォトリフレクタにあるか判定

  //戻り値は真ん中から数えて0～2（プラス）
  for (int i = 0; i <= 2; i++) {
    if (lineSensorR[i] < 100) {
      return i;
    }
  }

}

/*
  処理：ラインが何番目のフォトリフレクタの下にあるか判定
  引数：int ary[]: ラインセンサーのアナログ値が入った配列
  戻値：ラインセンサーの位置
*/

int getLsensor(int ary[])
{
  //ラインが何番目のフォトリフレクタ上にあるか判定
  //戻り値は真ん中から数えて0～-2（マイナス）
  for (int i = 0; i <= 2; i++) {
    if (lineSensorR[i] < 100) {
      return 0 - i;
    }
  }
}

//モータドライバ I2C制御 motor driver I2C
void writeMotorResister(int motor, byte vset, byte data1) {
  int vdata = vset << 2 | data1;
  Wire.beginTransmission(motor);
  Wire.write(0x00);
  Wire.write(vdata);
  Wire.endTransmission(true);
}

void setup()
{
  Wire.begin();
  Serial.begin(115200);             //シリアル通信開始
  pinMode(slaveSelectPin0, OUTPUT); // SS(10)を出力に設定
  pinMode(slaveSelectPin1, OUTPUT);
  // モーターの出力設定
  pinMode(34, INPUT);
  pinMode(35, INPUT);
  SPI.setBitOrder(MSBFIRST);           // MSBから送信
  SPI.setClockDivider(SPI_CLOCK_DIV2); //クロック分周設定
  SPI.setDataMode(SPI_MODE0);          // SPIモード0
  SPI.begin();                         // SPI通信開始
}

void loop()
{
  byte data[4] = {0, 0, 0, 0};
  for (int i = 0; i <= 2; i++)
  {
    get_adc(i);

    // lineSensorL,R配列にアナログ値を入れる
    lineSensorL[len - (i + 1)] = analogData[0];
    lineSensorR[i] = analogData[1];
  }
  // 真ん中のセンサーがおかしいので値をコピーする
  lineSensorR[0] = lineSensorL[2];

  for (int i = 0; i <= 2; i++) {
    Serial.print(lineSensorL[i]);
    Serial.print(", ");
  }
  for (int i = 0; i <= 2; i++) {
    Serial.print(lineSensorR[i]);
    Serial.print(", ");
  }
  Serial.println();
//  delay(2000);

  //マウスのラインセンサーのズレ位置から修正量を求める
  int lineOffsetL = int(getLsensor(lineSensorL) * Kp); //マイナス
  int lineOffsetR = int(getRsensor(lineSensorR) * Kp); //プラス

  Serial.println(lineOffsetL);
  Serial.println(lineOffsetR);
  delay(1000);

  //モーターの回転数を変えて真ん中にラインが来るように制御する
  writeMotorResister(DRV_ADR_L, MIN_VSET - lineOffsetL, FORWARD);
  writeMotorResister(DRV_ADR_R, MIN_VSET - lineOffsetR, FORWARD);

  //待ち
  delay(10);
}

void get_adc(byte ch)
{
  byte data[4] = {0, 0, 0, 0};              //SPI通信用変数

  //------[ ADC0のデータを取得する ]------
  digitalWrite(slaveSelectPin0, LOW);       //CS LOW
  SPI.transfer(0x01);                       //①スタートビット送信
  data[0] = SPI.transfer((ch << 4) | 0x80); //②Single-ended チャンネル選択,ADC0のbit9,8取得
  data[1] = SPI.transfer(0);                //③ADC0のbit7～0取得
  digitalWrite(slaveSelectPin0, HIGH);      //CS HIGH

  // ------[ ADC1のデータを取得する ]------
  digitalWrite(slaveSelectPin1, LOW);       //CS LOW
  SPI.transfer(0x01);                       //①スタートビット送信
  data[2] = SPI.transfer((ch << 4) | 0x80); //②Single-ended チャンネル選択,ADC0のbit9,8取得
  data[3] = SPI.transfer(0);                //③ADC0のbit7～0取得
  digitalWrite(slaveSelectPin1, HIGH);      //CS HIGH

  analogData[0] = ((data[0] & 0x03) << 8) | data[1];  //ADC0
  analogData[1] = ((data[2] & 0x03) << 8) | data[3];  //ADC1
}
