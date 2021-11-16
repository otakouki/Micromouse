/*
 *  ESP32 GPIO interrupt test
 *  2021.11.4
 *  ota koki
 */

#define LED_Pin 27
#define SW_Pin 13
volatile int state = LOW;

 
void setup() {
  pinMode(SW_Pin, INPUT_PULLUP);
  pinMode(LED_Pin, OUTPUT);
  attachInterrupt(SW_Pin, LED_blink, FALLING);
}

void loop() {
  digitalWrite(LED_Pin, state);
}

void LED_blink(){
//  delay(10);
  state = !state;
}
