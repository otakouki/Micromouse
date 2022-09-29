// 前期の成果発表の際に使用したコード

#include <Wire.h>
#include <SPI.h>
// I2Cアドレス
#define DRV_ADR_R 0x64 // RightDRV8830のI2Cアドレス
#define DRV_ADR_L 0x60 // LeftDRV8830のI2Cアドレス

// ブリッジ制御
#define STOP 0x00     //停止
#define FORWARD 0x01  //正転
#define BACKWARD 0x02 //逆転
#define BACKWARD 0x03 //ブレーキ

// 電圧定義
#define MAX_VSET 0x3F // 5.06V
#define MIN_VSET 0x09 // 0.72V
#define R_VSET 0x0D   // 2.97V
#define L_VSET 0x25   // 2.97V

// しきい値
#define TH_LVL 250

boolean startflag = false;
boolean moveflag = false;
boolean flag = false;
unsigned long previousMillis = 0;

#define slaveSelectPin0 4
#define slaveSelectPin1 5
int analogData[16];
int R_sns[6];
int R_sns_old[6];
int L_sns[6];
int L_sns_old[6];

void writeMotorResister(int motor, byte vset, byte dir)
{
  int vdata = vset << 2 | dir;
  Wire.beginTransmission(motor);
  Wire.write(0x00);
  Wire.write(vdata);
  Wire.endTransmission(true);
}
int16_t count = 0; //カウント数
unsigned long pulseCounterRight = 0;
unsigned long pulseCounterLeft = 0;

void onRisingRight()
{
  ++pulseCounterRight;
}

void onRisingLeft()
{
  ++pulseCounterLeft;
}

void setup()
{
  // put your setup code here, to run once:
  Wire.begin();

  // Serial.begin(115200);
  writeMotorResister(DRV_ADR_R, MIN_VSET, STOP);
  writeMotorResister(DRV_ADR_L, MIN_VSET, STOP);
  delay(100);

  // pinMode(34, INPUT);
  // attachInterrupt(digitalPinToInterrupt(34), onRisingRight, FALLING);
  // pinMode(35, INPUT);
  // attachInterrupt(digitalPinToInterrupt(35), onRisingLeft, FALLING);

  pinMode(slaveSelectPin0, OUTPUT); // SS(10)を出力に設定
  pinMode(slaveSelectPin1, OUTPUT);
  SPI.setBitOrder(MSBFIRST);           // MSBから送信
  SPI.setClockDivider(SPI_CLOCK_DIV2); //クロック分周設定
  SPI.setDataMode(SPI_MODE0);          // SPIモード0
  SPI.begin();                         // SPI通信開始
}

void loop()
{
  unsigned long currentMillis = millis();
  long times[] = {0};
  long spin;
  int i = 0;
  long timer;
  long starttime;
  long goaltime;
  get_adc();

  /***
    スタート・ゴール判定用
    スタートマーカーが認識：１
    ゴールマーカーが認識:０
  */
  static int forward = 0;

  /***
    コーナーマーカー認識しているかの判定
    コーナー開始：1
    コーナー終了:0
  */
  static int turn = 0;
  //スタート
  if (forward == 1)
  {
    // スタートしていない場合戻る
    if (flag == false)
    {
      return;
    }
    // ストップ処理
    if (R_sns[3] == 1 && micros() - starttime > 3000000)
    {
      goaltime = micros() - starttime;
      starttime = 0;
      forward = 0;
      writeMotorResister(DRV_ADR_R, MIN_VSET, STOP);
      writeMotorResister(DRV_ADR_L, MIN_VSET, STOP);
      flag = false;
    }
  }
  else
  {
    // スタート処理]
    if (startflag == false)
    {
      if (R_sns[3] == 1)
      {
        starttime = micros() - starttime;
        forward = 1;
        flag = true;
        writeMotorResister(DRV_ADR_R, MIN_VSET, FORWARD);
        writeMotorResister(DRV_ADR_L, MIN_VSET, FORWARD);
        i = 0;
      }
      startflag = true;
    }
    else
    {
      startflag = false;
    }
  }

  if (forward == 0)
  {
    return;
  }

  // 配列に1が格納されていると白判定。
  if (L_sns[1] == 1)
  {
    writeMotorResister(DRV_ADR_R, MIN_VSET + 6, FORWARD);
    writeMotorResister(DRV_ADR_L, MIN_VSET, FORWARD);
  }
  else if (L_sns[2] == 1)
  {
    writeMotorResister(DRV_ADR_R, MIN_VSET + 2, FORWARD);
    writeMotorResister(DRV_ADR_L, MIN_VSET, FORWARD);
  }
  else if (L_sns[3] == 1)
  {
    writeMotorResister(DRV_ADR_R, MIN_VSET, FORWARD);
    writeMotorResister(DRV_ADR_L, MIN_VSET, FORWARD);
  }
  else
  {
    writeMotorResister(DRV_ADR_R, MIN_VSET, STOP);
    writeMotorResister(DRV_ADR_L, MIN_VSET, FORWARD);
  }

  if (R_sns[2] == 1)
  {
    writeMotorResister(DRV_ADR_R, MIN_VSET, FORWARD);
    writeMotorResister(DRV_ADR_L, MIN_VSET + 6, FORWARD);
  }
  else if (R_sns[1] == 1)
  {
    writeMotorResister(DRV_ADR_R, MIN_VSET, FORWARD);
    writeMotorResister(DRV_ADR_L, MIN_VSET + 2, FORWARD);
  }
  else if (R_sns[0] == 1)
  {
    writeMotorResister(DRV_ADR_R, MIN_VSET, FORWARD);
    writeMotorResister(DRV_ADR_L, MIN_VSET, FORWARD);
  }
  else
  {
    writeMotorResister(DRV_ADR_R, MIN_VSET, FORWARD);
    writeMotorResister(DRV_ADR_L, MIN_VSET, STOP);
  }
}

void get_adc()
{
  for (int ch = 0; ch < 4; ch++)
  {
    byte data[4] = {0, 0, 0, 0}; // SPI通信用変数
    //------[ ADC0のデータを取得する ]------
    digitalWrite(slaveSelectPin0, LOW);       // CS LOW
    SPI.transfer(0x01);                       //①スタートビット送信
    data[0] = SPI.transfer((ch << 4) | 0x80); //②Single-ended チャンネル選択,ADC0のbit9,8取得
    data[1] = SPI.transfer(0);                //③ADC0のbit7～0取得
    digitalWrite(slaveSelectPin0, HIGH);      // CS HIGH

    // ------[ ADC1のデータを取得する ]------
    digitalWrite(slaveSelectPin1, LOW);       // CS LOW
    SPI.transfer(0x01);                       //①スタートビット送信
    data[2] = SPI.transfer((ch << 4) | 0x80); //②Single-ended チャンネル選択,ADC0のbit9,8取得
    data[3] = SPI.transfer(0);                //③ADC0のbit7～0取得
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
      if (ch == 3 && (int)analogData[0] <= 300)
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