#include <Wire.h>
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
#define MAX_VSET 0x23 // 5.06V
#define MIN_VSET 0x09 // 0.72V
//#define R_VSET 0x0D // 2.97V
//#define L_VSET 0x25 // 2.97V
#define CSPEED 0x23 //2.81V
boolean flag = false;
boolean flag2 = false;
byte constspeed = 0x23;
// 制御コマンド送信
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
void faultreset(byte DRV_ADR) {
  //DRV8830 FAULTクリア
  Wire.beginTransmission(DRV_ADR);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(FLT_ADR);//フォルト(障害状態)
  Wire.write(0x80);//フォルトクリア
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了
}
void setup() {
  Wire.begin();
  Serial.begin(115200);
  controlreset(DRV_ADR_R);
  controlreset(DRV_ADR_L);
  faultreset(DRV_ADR_R);
  faultreset(DRV_ADR_L);
  delay(100);
}
void loop() {
  if (flag == false) {
    //モータドライバブリッジ制御をスタンバイ、最低電圧に設定
    write_vset_l(MIN_VSET, M_STANBY);
    write_vset_r(MIN_VSET, M_STANBY);
    delay(1000);
    flag == true;
  }
  delay(1000);
  write_vset_l(0x10, 0x2);
  write_vset_r(0x10, 0x2);
  delay(5000);
  write_vset_l(0x10, 0x0);
  write_vset_r(0x10, 0x0);
  delay(5);
  // for (byte i = MIN_VSET; i <= CSPEED; i+=9) {
  // write_vset_l(i, M_NORMAL);
  // write_vset_r(i, M_NORMAL);
  // delay(1000);
  // }
}
