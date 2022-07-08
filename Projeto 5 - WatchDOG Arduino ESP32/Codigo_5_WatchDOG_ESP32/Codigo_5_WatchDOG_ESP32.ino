#include "esp_system.h"
int led = 15;
const int button = 13;         //gpio to use to trigger delay
const int wdtTimeout = 3000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("running setup");
  pinMode(led,OUTPUT);
  pinMode(button, INPUT_PULLUP);                    //init control pin
  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt

  digitalWrite(led,HIGH);
  delay(500);
  digitalWrite(led,LOW);
}

void loop() {
  Serial.println("ESp32 acordado");

  timerWrite(timer, 0); //reset timer (feed watchdog)
  long loopTime = millis();
  //while button is pressed, delay up to 3 seconds to trigger the timer
  while (!digitalRead(button)) {
    Serial.println("ESP32 travado!");
    delay(500);
  }
  delay(1000); //simulate work
  loopTime = millis() - loopTime;
  
  Serial.print("Tempo de Loop= ");
  Serial.println(loopTime); //should be under 3000
}
