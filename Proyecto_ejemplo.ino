#include "OTAUpdater.h"

//Pines ejemplo con LEDS.............................
#define PIN_RESET 12
#define LED1 15
#define LED2 4

void setup() {
  Serial.begin(115200);
  //Configuración ejemplo LEDS.................
  pinMode(PIN_RESET, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  digitalWrite(LED1, HIGH); 
  digitalWrite(LED2, LOW);
//.................................
  OTA_begin();  // Comprobar al arranque
}

void loop() {
  //Ejemplo LEDS.......................
  if (digitalRead(PIN_RESET) == LOW) {
    delay(300);
    esp_restart();
  }
  //...................................

  OTA_check();  // Comprobar cada minuto
}