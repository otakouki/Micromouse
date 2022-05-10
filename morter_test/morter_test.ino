//esp8266でWire.hでI2C通信テスト。1コだけ。
#include <Wire.h>;
const int address = 0x60;
const int address2 = 0x64;

void writeRegister(int adr,byte reg,byte vset,byte data){
  int vdata = vset << 2 | data;
  Wire.beginTransmission(adr);
  Wire.write(reg);
  Wire.write(vdata);
  Wire.endTransmission(true);
  Serial.print(adr,HEX);
  Serial.print(reg,HEX);
  Serial.print(vdata,HEX);
  delay(12);
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(4,14);
  Serial.println(address,HEX);
  Serial.println("Hello");
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  writeRegister(address,0x00,0x25,0x01);//正転
  writeRegister(address2,0x00,0x25,0x01);//正転
  delay(1000);
  writeRegister(address,0x00,0x25,0x00);//静止
  delay(1000);
  writeRegister(address,0x00,0x25,0x02);//逆転
  delay(1000);
  writeRegister(address,0x00,0x25,0x00);//静止
  delay(1000);

  Serial.println("sendOK");
}
