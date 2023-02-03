/**
 * @file MouseElectricityCheck.ino
 * @brief 基盤の通電チェック
 * @author ota koki
 * @date 2022/12/8
 */

// LED用の番号
#define LED 25
void setup()
{
  // LEDピンの出力設定
  pinMode(LED, OUTPUT);
  // シリアル通信のデータ転送レートをbps(baud)で指定します
  Serial.begin(115200);
}
void loop()
{
  /**
   * digitalWrite(PIN,value)
   * 　@param PIN番号
   * 　@param HIGH or LOW
   * 　@return なし
   */
  digitalWrite(LED, HIGH);
  Serial.println("HIGH");
  delay(100);
  Serial.println("LOW");
  digitalWrite(LED, LOW);
  delay(300);
}
