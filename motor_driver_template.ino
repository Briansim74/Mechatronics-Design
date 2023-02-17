#include <ESP32Encoder.h>
#define BIN_1 26
#define BIN_2 25
#define LED_PIN 13

ESP32Encoder encoder;

//Setup interrupt variables ----------------------------
volatile int count = 0; // encoder count
volatile bool interruptCounter = false;    // check timer interrupt 1
volatile bool deltaT = false;     // check timer interrupt 2
int totalInterrupts = 0;   // counts the number of triggering of the alarm
hw_timer_t * timer0 = NULL;
hw_timer_t * timer1 = NULL;
portMUX_TYPE timerMux0 = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE timerMux1 = portMUX_INITIALIZER_UNLOCKED;

// setting PWM properties ----------------------------
const int freq = 5000;
const int ledChannel_1 = 1;
const int ledChannel_2 = 2;
const int resolution = 8;
int MAX_PWM_VOLTAGE = 255;
int motor_PWM;

// encoder properties ------------------------------
int v = 0;


//Initialization ------------------------------------
void IRAM_ATTR onTime0() {
  portENTER_CRITICAL_ISR(&timerMux0);
  interruptCounter = true; // the function to be called when timer interrupt is triggered
  portEXIT_CRITICAL_ISR(&timerMux0);
}

void IRAM_ATTR onTime1() {
  portENTER_CRITICAL_ISR(&timerMux1);
  count = encoder.getCount( );
  encoder.clearCount ( );
  deltaT = true; // the function to be called when timer interrupt is triggered
  portEXIT_CRITICAL_ISR(&timerMux1);
}


void setup() {
  pinMode(LED_PIN, OUTPUT); // configures the specified pin to behave either as an input or an output
  digitalWrite(LED_PIN, LOW); // sets the initial state of LED as turned-off
  
  Serial.begin(115200);
  ESP32Encoder::useInternalWeakPullResistors = UP; // Enable the weak pull up resistors
  encoder.attachHalfQuad(27, 33); // Attache pins for use as encoder pins
  encoder.setCount(0);  // set starting count value after attaching

  // configure LED PWM functionalitites
  ledcSetup(ledChannel_1, freq, resolution);
  ledcSetup(ledChannel_2, freq, resolution);

  // attach the channel to the GPIO to be controlled 
  ledcAttachPin(BIN_1, ledChannel_1);
  ledcAttachPin(BIN_2, ledChannel_2);

  // initialize timer
  timer0 = timerBegin(0, 80, true);  // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer0, &onTime0, true); // edge (not level) triggered 
  timerAlarmWrite(timer0, 2000000, true); // 2000000 * 1 us = 2 s, autoreload true

  timer1 = timerBegin(1, 80, true);  // timer 1, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer1, &onTime1, true); // edge (not level) triggered 
  timerAlarmWrite(timer1, 10000, true); // 10000 * 1 us = 10 ms, autoreload true

  // at least enable the timer alarms
  timerAlarmEnable(timer0); // enable
  timerAlarmEnable(timer1); // enable
}

void loop() {
  if (interruptCounter) {
    portENTER_CRITICAL(&timerMux0);
    interruptCounter = false;
    portEXIT_CRITICAL(&timerMux0);
    
    totalInterrupts++;

    if ( totalInterrupts%2 == 0) {
      digitalWrite(LED_PIN, HIGH);
      ledcWrite(ledChannel_1, LOW);
      ledcWrite(ledChannel_2, MAX_PWM_VOLTAGE);
      motor_PWM = MAX_PWM_VOLTAGE;
    } else {
      digitalWrite(LED_PIN, LOW);
      ledcWrite(ledChannel_2, LOW);
      ledcWrite(ledChannel_1, MAX_PWM_VOLTAGE);
      motor_PWM = -MAX_PWM_VOLTAGE;
    }
  }
  if (deltaT) {
    portENTER_CRITICAL(&timerMux1);
    deltaT = false;
    portEXIT_CRITICAL(&timerMux1);

    // RESET deltaT FLAG HERE (hint: see the 3 lines after if(interruptCounter))

    v = count;

    Serial.println("motor_PWM, speed");
    Serial.print(motor_PWM);
    Serial.print(" ");
    Serial.println(v);
    
  }
}
