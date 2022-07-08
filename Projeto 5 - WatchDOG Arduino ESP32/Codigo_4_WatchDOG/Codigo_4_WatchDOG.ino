#include <avr/wdt.h>

int led = 3;
int botao = 2;
int estado_botao;

void setup() {
  //pulsante collegato tra pin 3 e GND
  pinMode(botao, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  //il Watchdog resta attivo dopo il reset e, se non disabilitato, 
  //esso può provocare il reset perpetuo del microcontrollore
  wdt_disable();
  Serial.println("Cão-de-Guarda em ação!!");
  Serial.println("Arduino acordado!");
  wdt_enable(WDTO_4S);

  digitalWrite(led,HIGH);
  delay(500);
  digitalWrite(led,LOW);
}
  
void loop() {
  estado_botao = HIGH;
  while (estado_botao) {
    estado_botao = digitalRead(botao);
    delay(100);
    Serial.print(".");
  }
  Serial.println("Arduino acordado!");
  wdt_reset();
}
