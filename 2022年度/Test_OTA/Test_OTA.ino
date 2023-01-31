/**
  ＜＜OTAテストプログラム＞＞ 2022/12/26 by 藤原 
  ・OTAに必要なのは次の3行
      ①#include <ArduinoOTA.h>
      ②setup()でArduinoOTA.begin();
      ③loop()で毎回ArduinoOTA.handle();
   ※本ソースコードを書き込んだ後は、Arduino IDEの接続手段にシリアルポートに
      BlinkLED_OTA at 192.168.0.xx(ESP32 Dev Module)が現れるようになる。
      それを選択してマイコンボードに書き込みを行う。現れない場合はArduinoIDEを再起動する。
   ※Sending invitation to 192.168.0.12 .........スケッチの書き込み中にエラーが発生しました
     [ERROR]: No response from the ESP
     が出る場合はPCのファイアウォールを一時的に停止すること。
   ※参考URLは　https://diysmarthome.hatenablog.com/entry/2022/12/04/184255
 */

#include <WiFi.h>
#include <ArduinoOTA.h>   //①

#define LED 25
const char* ssid = "CampusIoT-WiFi";
const char* password = "0b8b413f2c0fa6aa90e085e9431abbf1fa1b2bd2db0ecf4ae9ce4b2e87da770c";

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
//    delay(5000);
//    ESP.restart();
    break;  //処理続行
  }

  Serial.print("WiFi connected and IP address:");
  Serial.println(WiFi.localIP());

  ArduinoOTA.setHostname("MicroMouse");
  ArduinoOTA.begin();   //②
  pinMode(LED,OUTPUT);
}

void loop() {
  ArduinoOTA.handle();  //③
  Serial.println("test");
  digitalWrite(LED, HIGH); 
  delay(1000);  
  digitalWrite(LED, LOW);
  delay(1000);
}
