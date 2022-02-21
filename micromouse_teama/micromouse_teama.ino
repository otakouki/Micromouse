#include <Wire.h>

// I2Cアドレス
#define DRV_ADR_R   0x64  // RightDRV8830のI2Cアドレス
#define DRV_ADR_L   0x60  // LeftDRV8830のI2Cアドレス

// ブリッジ制御
#define STOP      0x00  //停止
#define FORWARD   0x01  //正転
#define BACKWARD  0x02  //逆転
#define BACKWARD  0x03  //ブレーキ

// 電圧定義
#define MAX_VSET 0x3F   // 5.06V
#define MIN_VSET 0x09   // 0.72V
#define R_VSET 0x0D  // 2.97V
#define L_VSET 0x25  // 2.97V

#include <SPI.h>
#define slaveSelectPin0 4
#define slaveSelectPin1 5

int analogData[16];

//**********************************************************************
//モータドライバ I2C制御 motor driver I2C
//  Given:  int motor: モータドライバICのアドレス(motorL:0x60、motorR:0x64)
//          byte vset: モーター駆動電圧(0x00-0x3F)
//          byte dir: モーターの回転方向(STOP:0x00:停止、FORWARD:0x01:正転、BACKWARD:0x02:逆転)
//  return: なし
//http://makers-with-myson.blog.so-net.ne.jp/2014-05-15
//**********************************************************************
void writeMotorResister(int motor, byte vset, byte dir) {
  int vdata = vset << 2 | dir;
  Wire.beginTransmission(motor);
  Wire.write(0x00);
  Wire.write(vdata);
  Wire.endTransmission(true);
}

void setup() {
  Wire.begin();
  Serial.begin(115200);
  pinMode(slaveSelectPin0, OUTPUT);          //SS(4)を出力に設定
  pinMode(slaveSelectPin1, OUTPUT);          //SS(5)を出力に設定
  SPI.setBitOrder(MSBFIRST);                //MSBから送信
  SPI.setClockDivider(SPI_CLOCK_DIV2);      //クロック分周設定
  SPI.setDataMode(SPI_MODE0);               //SPIモード0
  SPI.begin();                              //SPI通信開始
}



void loop() {
  int lr = get_Rsns(0, "l");      //コーナー取得用の変数
  int sta = get_Rsns(0, "r");     //スタートストップ用の変数
  /*** 
   *  left1~4:左寄りか判定
   *  left5right5:真っ直ぐになっているか
   *  right1~4:右寄りか判定
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
   * スタート・ゴール判定用
   * スタートマーカーが認識：１
   * ゴールマーカーが認識:０
   */
  static int forward = 0;
  
  /***
   * コーナーマーカー認識しているかの判定
   * コーナー開始：1
   * コーナー終了:0
   */
  static int turn = 0;
  
  if (forward == 1) {
    if (turn == 1) {
      if (left1 == 1 || left2 == 1) {
        //if(left5 == 1){
        Serial.print("left start");
        writeMotorResister(DRV_ADR_L, MIN_VSET + 4, FORWARD);
        writeMotorResister(DRV_ADR_R, MIN_VSET + 2, FORWARD);
        delay(50);
      }
      if (right1 == 1 || right2 == 1) {
        //if(right5 == 1){
        Serial.print("right start");
        writeMotorResister(DRV_ADR_L, MIN_VSET + 2, FORWARD);
        writeMotorResister(DRV_ADR_R, MIN_VSET + 4, FORWARD);
        delay(50);
      }
      if (lr == 1) {
        writeMotorResister(DRV_ADR_L, MIN_VSET + 2, FORWARD);
        writeMotorResister(DRV_ADR_R, MIN_VSET + 2, FORWARD);
        Serial.println("turn end");
        delay(1000);
        turn = 0;
      }
    } else {
      if (lr == 1) {
        Serial.println("ln start");
        delay(1000);
        turn = 1;
      }
    }
    if (sta == 1) {
      Serial.print("stop");
      writeMotorResister(DRV_ADR_L, MIN_VSET, STOP);
      writeMotorResister(DRV_ADR_R, MIN_VSET, STOP);
      delay(3000);
      forward = 0;
    }
  } else {
    //スタート
    if (sta == 1) {

      Serial.print("start");
      writeMotorResister(DRV_ADR_L, MIN_VSET + 2, FORWARD);
      writeMotorResister(DRV_ADR_R, MIN_VSET + 2, FORWARD);
      delay(1000);
      //delay(50);
      forward = 1;

    }
  }
  delay(100000);
}

/***
 * 
 */
void get_adc(int ch) {
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
 * ch:チャンネル番号
 * leri:右ならr,左ならl
 * analogDataが100以下の時:1
 * analogDataが100以上の時:0
 */
int get_Rsns(int ch, String leri) {
  get_adc(ch);
  int r_analogData = analogData[0]; //ADC0
  int l_analogData = analogData[1]; //ADC1

  if (leri == "r") {
    if (r_analogData <= 100) {
      Serial.println("right" + String(ch) + String(1));
      return 1;
    } else {
      Serial.println("right" + String(ch) + String(0));
      return 0;
    }
  }
  if (leri == "l") {
    if (l_analogData <= 100) {
      Serial.println("left" + String(ch) + String(1));
      return 1;
    } else {
      Serial.println("left" + String(ch) + String(0));
      return 0;
    }
  }
}
