#define OTA
/**
   @file TB6612FNG.ino
   @brief モーターが動くかチェック
   @author ota koki
   @date 2022/12/13
   変更日
    2023/1/26 by ota koki
     モーターが回るか確認した
    2022/12/26 by fujihara
      OTA処理を追加
        ①#include <ArduinoOTA.h>
        ②setup()でArduinoOTA.begin();
        ③loop()で毎回ArduinoOTA.handle();
    2022/12/15
      初版
*/

#ifdef OTA
#include <WiFi.h>
#include <ArduinoOTA.h> //①
#endif

// LED用の番号
#define LED 25
const int CHANNEL_A = 0;
const int CHANNEL_B = 1;

const int LEDC_TIMER_BIT = 8;
const int LEDC_BASE_FREQ = 490;

const int motorA[3] = {32, 33, 14}; // AIN1, AIN2, PWMA
const int motorB[3] = {16, 17, 12}; // BIN1, BIN2, PWMB

#ifdef OTA
const char *ssid = "CampusIoT-WiFi";
const char *password = "0b8b413f2c0fa6aa90e085e9431abbf1fa1b2bd2db0ecf4ae9ce4b2e87da770c";
#endif

void setup()
{
  Serial.begin(115200);

#ifdef OTA
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("Wi-Fi接続開始");

  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Wi-Fi接続失敗で続行");
  }
  else
  {
    Serial.print("Wi-Fi接続成功。IP address:");
    Serial.println(WiFi.localIP());

    ArduinoOTA.setHostname("TB6612FNG");
    ArduinoOTA.begin(); // ②
  }
#endif

  // LEDピンの出力設定
  pinMode(LED, OUTPUT);

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

#ifdef OTA
  ArduinoOTA.handle(); // ③
#endif

  Serial.println("正回転します。");
  delay(100);

  // モーターを止める
  for (int i = 0; i < 2; i++)
  {
    digitalWrite(motorA[i], HIGH);
    digitalWrite(motorB[i], HIGH);
  }

  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);

  // 左モータ（CCW，時計回り）
  digitalWrite(motorA[1], HIGH);
  digitalWrite(motorA[0], LOW);
  ledcWrite(CHANNEL_A, 100);

  // 右モータ（CW，時計回り）
  digitalWrite(motorB[1], LOW);
  digitalWrite(motorB[0], HIGH);
  ledcWrite(CHANNEL_B, 100);

  delay(1000);
  digitalWrite(LED, HIGH);

  // 左モータ（CCW，時計回り）
  digitalWrite(motorA[1], HIGH);
  digitalWrite(motorA[0], LOW);
  ledcWrite(CHANNEL_A, 150);

  // 右モータ（CW，時計回り）
  digitalWrite(motorB[1], LOW);
  digitalWrite(motorB[0], HIGH);
  ledcWrite(CHANNEL_B, 150);

  delay(1000);
  digitalWrite(LED, LOW);

  // 左モータ（CCW，時計回り）
  digitalWrite(motorA[1], HIGH);
  digitalWrite(motorA[0], LOW);
  ledcWrite(CHANNEL_A, 200);

  // 右モータ（CW，時計回り）
  digitalWrite(motorB[1], LOW);
  digitalWrite(motorB[0], HIGH);
  ledcWrite(CHANNEL_B, 200);

  digitalWrite(motorA[1], HIGH);
  digitalWrite(motorA[0], LOW);
  ledcWrite(CHANNEL_A, 250);

  // 右モータ（CW，時計回り）
  digitalWrite(motorB[1], LOW);
  digitalWrite(motorB[0], HIGH);
  ledcWrite(CHANNEL_B, 250);

  delay(1000);
  digitalWrite(LED, HIGH);
}
