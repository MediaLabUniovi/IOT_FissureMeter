#include "configuration.h"

//TwoWire myWire(BME_SDA, BME_SCL); // SDA, SCL
Adafruit_BME280 bme280;

const unsigned long Intervalo = 5000;

// ===========================================================================================================================================================
// Setup main
// ===========================================================================================================================================================
void setup() {
  #if ENABLE_DEBUG == 1
    Serial.begin(115200);
  #endif
  delay(10);

  if (bme280.begin(0x3C)) {
    Serial.println("BME280 Iniciado");
  }
  delay(100);

  dTemp.setResolution(10);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);  // de 0.15V a 3.3 V error de 0.1 V

  pinMode(PIN_EN_CLK, INPUT_PULLUP);
  pinMode(PIN_EN_DT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_EN_CLK), encoderA, CHANGE);  // Interrupción en cambios
  attachInterrupt(digitalPinToInterrupt(PIN_EN_DT), encoderB, CHANGE);     // Interrupción en cambios

  pinMode(PIN_BAT, INPUT);
  pinMode(PIN_HUM, INPUT);
  pinMode(PIN_LLUVIA, INPUT);

  Serial.println("Setup acabado");
}

// ===========================================================================================================================================================
// Loop main
// ===========================================================================================================================================================
void loop() {


  getBME();
  getDallasTemp();
  getHum();
  getRain();
  getBat();
  Serial.print("Coder: ");
  Serial.println(counter);
  Serial.println("");

  delay(5000);
}
