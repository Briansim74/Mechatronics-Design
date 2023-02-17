#include <Arduino.h>
#define BTN 27  // declare the button ED pin number
#define SPK 25  // declare the speaker pin number
#define LED_PIN 13 // declare the builtin LED pin number

//Setup variables ------------------------------------
const int freq = 5000;
const int pwmChannel = 0;
const int resolution = 8;
volatile bool buttonIsPressed = false;
int state = 1;

//Initialization ------------------------------------
void IRAM_ATTR isr() {  // the function to be called when interrupt is triggered
  buttonIsPressed = true;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(BTN, INPUT);  // configures the specified pin to behave either as an input or an output
  pinMode(LED_PIN, OUTPUT); 
  attachInterrupt(BTN, isr, CHANGE);

  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(SPK, pwmChannel);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
  switch (state) {
    case 1:
      if (CheckForButtonPress() == true) {
        led_on();
        sound_on();
        state = 2;
      }
      break;
      
    case 2:
      if (CheckForButtonPress() == true) {
        led_off();
        sound_off();
        state = 1;
      }
      break;
  }
}

bool CheckForButtonPress() {
  if (buttonIsPressed == true){
    buttonIsPressed = false;
    return true;
  }
  else {
    return false;
  }
}

void sound_on() {
  ledcAttachPin(SPK, pwmChannel);
  ledcWriteTone(pwmChannel, 440);
}

void sound_off() {
  ledcDetachPin(SPK);
  ledcWriteTone(pwmChannel, 0);
}

void led_on() {
  digitalWrite(LED_PIN, HIGH);
}

void led_off() {
  digitalWrite(LED_PIN, LOW);
}
