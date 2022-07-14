#include <Wire.h>
#include <SPI.h>
// I2Cアドレス
#define DRV_ADR_R 0x64 // RightDRV8830のI2Cアドレス
#define DRV_ADR_L 0x60 // LeftDRV8830のI2Cアドレス
#define CTR_ADR 0x00 // CONTROLレジスタのサブアドレス
#define FLT_ADR 0x01 // FAULTレジスタのアドレス
// ブリッジ制御
#define M_STANBY B00 // スタンバイ
#define M_REVERSE B01 // 逆転
#define M_NORMAL B10 // 正転
#define M_BRAKE B11 // ブレーキ
// 電圧定義
#define MAX_VSET 0x29 // 3.29V
#define MIN_VSET 0x09 // 0.72V
#define CSPEED 0x23 //2.81V

boolean startflag = false;
boolean moveflag = false;

#define slaveSelectPin0 4
#define slaveSelectPin1 5
int analogData[16];

int write_vset_r(byte vs, byte ctr) {
  Wire.beginTransmission(DRV_ADR_R);
  Wire.write(CTR_ADR);
  Wire.write(vs << 2 | ctr);
  return Wire.endTransmission();
}

// 制御コマンド送信
int write_vset_l(byte vs, byte ctr) {
  Wire.beginTransmission(DRV_ADR_L);
  Wire.write(CTR_ADR);
  Wire.write(vs << 2 | ctr);
  return Wire.endTransmission();
}

void controlreset(byte DRV_ADR) {
  //DRV8830 CONTROLクリア
  Wire.beginTransmission(DRV_ADR);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(CTR_ADR);//コントロール
  Wire.write(0x00 << 2 | 0x00);//コントロールクリア
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了
}

void breaki(byte DRV_ADR) {
  //DRV8830 CONTROLクリア
  Wire.beginTransmission(DRV_ADR);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(CTR_ADR);//コントロール
  Wire.write(0x00 << 2 | 0x03);//ブレーキ
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了
}

void faultreset(byte DRV_ADR) {
  //DRV8830 FAULTクリア
  Wire.beginTransmission(DRV_ADR);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(FLT_ADR);//フォルト(障害状態)
  Wire.write(0x80);//フォルトクリア
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了
}

int16_t count = 0; //カウント数
unsigned long pulseCounterRight = 0;
unsigned long pulseCounterLeft = 0;

void onRisingRight() {
  ++pulseCounterRight;
}

void onRisingLeft() {
  ++pulseCounterLeft;
}

void setup() {

  // put your setup code here, to run once:
  Wire.begin();

  Serial.begin(115200);
  controlreset(DRV_ADR_R);
  controlreset(DRV_ADR_L);
  faultreset(DRV_ADR_R);
  faultreset(DRV_ADR_L);
  delay(100);


  pinMode(34, INPUT);
  attachInterrupt(digitalPinToInterrupt(34), onRisingRight, FALLING);
  pinMode(35, INPUT);
  attachInterrupt(digitalPinToInterrupt(35), onRisingLeft, FALLING);

  pinMode(slaveSelectPin0, OUTPUT);          //SS(10)を出力に設定
  pinMode(slaveSelectPin1, OUTPUT);
  SPI.setBitOrder(MSBFIRST);                //MSBから送信
  SPI.setClockDivider(SPI_CLOCK_DIV2);      //クロック分周設定
  SPI.setDataMode(SPI_MODE0);               //SPIモード0
  SPI.begin();                              //SPI通信開始

}

void loop() {
  static unsigned long times[] = {0};  //コーナー間の時間
  static unsigned long spin; //　コーナー開始時間
  static unsigned long timer;  //コーナー終了時間
  static unsigned long starttime = micros();  //スタート時間
  static unsigned long goaltime = 0; //ゴール時間
  static int i = 0; //配列に入れる要素番号
  static boolean flag = false;  //スタートしているかの判定

  int lr = get_Rsns(0, "l");      //コーナー取得用の変数
  int sta = get_Rsns(0, "r");     //スタートストップ用の変数
  /***
      left1~4:左寄りか判定
      left5right5:真っ直ぐになっているか
      right1~4:右寄りか判定
  */
  int left1 = get_Rsns(1, "l");
  int left2 = get_Rsns(2, "l");
  int left3 = get_Rsns(3, "l");
  int left4 = get_Rsns(4, "l");
  int left5 = get_Rsns(5, "l");
  int right1 = get_Rsns(1, "r");
  int right2 = get_Rsns(2, "r");
  int right3 = get_Rsns(3, "r");
  int right4 = get_Rsns(4, "r");
  int right5 = get_Rsns(5, "r");


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

  //スタート・ゴール判定
  if (forward == 1) {
    // スタートしていない場合戻る
    if (flag == false) {
      return;
    }
    // ストップ処理
    if (sta == 1 && micros() - starttime > 3000000) {
      Serial.println("stop");
      goaltime = micros() - starttime;
      starttime = 0;
      forward = 0;
      breaki(DRV_ADR_R);
      breaki(DRV_ADR_L);
      flag = false;
    }
  } else {
    // スタート処理]
    if (startflag == false) {
      if (sta == 1) {
        Serial.println("start" + (String)starttime);
        starttime = micros() - starttime;
        forward = 1;
        flag = true;
        write_vset_l(0x10, M_NORMAL);
        write_vset_r(0x10, M_NORMAL);
      }
      startflag = true;
    }else{
      startflag = false;
    }
  }
  if (forward == 1) {
    if (turn == 1) {
      if (lr == 1) {
        Serial.println("turn end");
        turn = 0;
        timer = micros() - timer;
        times[i] = timer;
        timer = micros();
        i++;
      }
    } else {
      if (lr == 1) {
        Serial.println("turn start");
        if (i == 0) {
          spin = micros() - starttime;
          timer = micros();
          times[i] = spin;
        } else {
          timer = micros() - timer;
          times[i] = timer;
          timer = micros();
          i++;
        }
        turn = 1;
        i++;
      }
    }
  }
}

void get_adc(byte ch) {
  byte data[4] = {0, 0, 0, 0};                               //SPI通信用変数
  //------[ ADC0のデータを取得する ]------
  digitalWrite(slaveSelectPin0, LOW);       //CS LOW
  SPI.transfer(0x01);                       //①スタートビット送信
  data[0] = SPI.transfer((ch << 4) | 0x80);  //②Single-ended チャンネル選択,ADC0のbit9,8取得
  data[1] = SPI.transfer(0);                //③ADC0のbit7～0取得
  digitalWrite(slaveSelectPin0, HIGH);                  //CS HIGH



  // ------[ ADC1のデータを取得する ]------
  digitalWrite(slaveSelectPin1, LOW);       //CS LOW
  SPI.transfer(0x01);                       //①スタートビット送信
  data[2] = SPI.transfer((ch << 4) | 0x80);  //②Single-ended チャンネル選択,ADC0のbit9,8取得
  data[3] = SPI.transfer(0);                //③ADC0のbit7～0取得
  digitalWrite(slaveSelectPin1, HIGH);                  //CS HIGH

  analogData[0] = ((data[0] & 0x03) << 8) | data[1];  //ADC0
  analogData[1] = ((data[2] & 0x03) << 8) | data[3];  //ADC1
}

/***
   ch:チャンネル番号
   leri:右ならr,左ならl
   analogDataが100以下の時:1
   analogDataが100以上の時:0
*/
int get_Rsns(int ch, String leri) {
  get_adc(ch);
  int r_analogData = analogData[0]; //ADC0
  int l_analogData = analogData[1]; //ADC1
  //Serial.println(r_analogData);
  if (leri == "r") {
    //    Serial.println(r_analogData);
    if (r_analogData <= 300) {
      return 1;
    } else {
      return 0;
    }
  }
  if (leri == "l") {
    //    Serial.println(l_analogData);
    if (l_analogData <= 300) {
      return 1;
    } else {
      return 0;
    }
  }
}
