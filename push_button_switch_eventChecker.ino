#include <Arduino.h>

//Define constants ------------------------------------
#define BTN 13  // declare the button ED pin number

//Setup variables ------------------------------------
volatile bool buttonIsPressed = false;
volatile bool timerflag = false;

//Initialization ------------------------------------
hw_timer_t * timer0 = NULL;
portMUX_TYPE timerMux0 = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR isr() {  // the function to be called when interrupt is triggered
  timerflag = false;
  buttonIsPressed = true;
  timerStart(timer0);
}

void IRAM_ATTR onTime0() {
  portENTER_CRITICAL_ISR(&timerMux0);
  timerflag = true; // the function to be called when timer interrupt is triggered
  portEXIT_CRITICAL_ISR(&timerMux0);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(BTN, INPUT);  // configures the specified pin to behave either as an input or an output
  attachInterrupt(BTN, isr, RISING);  // set the "BTN" pin as the interrupt pin; call function named "isr" when the interrupt is triggered; "Rising" means triggering interrupt when the pin goes from LOW to HIGH
  Serial.begin(115200);


  timer0 = timerBegin(0, 80, true);  // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer0, &onTime0, true); // edge (not level) triggered
  timerAlarmWrite(timer0, 100000, true); // 100000 * 1 us = 100 ms, autoreload true
  timerAlarmEnable(timer0); // enable

}

//Main loop ------------------------------------
void loop() {
  // put your main code here, to run repeatedly:

if (CheckForButtonPress()){
  ButtonResponse();
}

}

bool CheckForButtonPress() {

if((buttonIsPressed == true) & (timerflag == true)) {
  void timerRestart(hw_timer_t *timer);
  return true;
}

else {
  return false;
}
}

void ButtonResponse(){

buttonIsPressed = false;
Serial.println("Pressed!");
  
}
