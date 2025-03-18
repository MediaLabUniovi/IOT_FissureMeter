/* ***********************************************************************************************************************************************************
Lora to Do - MEDIALAB LPWAN UNIVERSIDAD DE OVIEDO
*********************************************************************************************************************************************************** */
#include "configuration.h"
#include "credentials.h"
#include "lmic_project_config.h"
#include "rom/rtc.h"

static uint8_t txBuffer[TX_BUFFER_SIZE];      // Enter the length of the payload in bytes (this has to be more than 3 if you want to receive downlinks)
unsigned long previousMillis = 0;             // Save the time from previous send
boolean firstTime = true;

Adafruit_BME280 bme280;
OneWire oneWireTemp(PIN_DS18); // Objeto del sensor de temperatura en pin del sensor
DallasTemperature dTemp(&oneWireTemp); // Inicialización del objeto

// -----------------------------------------------------------------------------------------------------------------------------------------------------------
// Funcion para enviar el paquete de datos LoRa
// -----------------------------------------------------------------------------------------------------------------------------------------------------------

void loraSend(){

  #if LORAWAN_CONFIRMED_EVERY > 0
    bool confirmed = (count % LORAWAN_CONFIRMED_EVERY == 0);
  #else
    bool confirmed = false;
  #endif

  ttn_send(txBuffer, sizeof(txBuffer), LORAWAN_PORT, confirmed);
}



// -----------------------------------------------------------------------------------------------------------------------------------------------------------
// Funcion para dormir el ESP32 durante el tiempo de duty cycle
// -----------------------------------------------------------------------------------------------------------------------------------------------------------
void sleep(){
  #if SLEEP_BETWEEN_MESSAGES == 1
    Serial.println("Iniciando deep sleep...");                // Mostrar mensaje de que se ha iniciado el deep sleep
  
    uint32_t sleep_for = (millis() < SEND_INTERVAL ? SEND_INTERVAL - millis() : SEND_INTERVAL);
    sleep_millis(sleep_for);
  #endif
}



// -----------------------------------------------------------------------------------------------------------------------------------------------------------
// Funcion para mostrar mensajes por monitor serial segun se interactue con TTN
// -----------------------------------------------------------------------------------------------------------------------------------------------------------
void callback(uint8_t message){
  if(EV_JOINING == message)Serial.println("Joining TTN...");
  if(EV_JOINED == message)Serial.println("TTN joined!");
  if(EV_JOIN_FAILED == message)Serial.println("TTN join failed");
  if(EV_REJOIN_FAILED == message)Serial.println("TTN rejoin failed");
  if(EV_RESET == message)Serial.println("Reset TTN connection");
  if(EV_LINK_DEAD == message)Serial.println("TTN link dead");
  if(EV_ACK == message)Serial.println("ACK received");
  if(EV_PENDING == message)Serial.println("Message discarded");
  if(EV_QUEUED == message)Serial.println("Message queued");

  if(EV_TXCOMPLETE == message){
    Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
    if(LMIC.txrxFlags & TXRX_ACK){
      Serial.println(F("Received ack"));
    }
    if(LMIC.dataLen){
      Serial.print(F("Received "));
      Serial.print(LMIC.dataLen);
      Serial.println(F(" bytes of payload"));
      for (int i = 0; i < LMIC.dataLen; i++){
        if (LMIC.frame[LMIC.dataBeg + i] < 0x10){
          Serial.print(F("0"));
        }
        Serial.print(F("Received payload: "));
        Serial.print(LMIC.frame[LMIC.dataBeg + i], HEX);
      }
      Serial.println();
    }
    sleep();
  }

  if(EV_RESPONSE == message){
    Serial.print(F("[TTN] Response: "));

    size_t len = ttn_response_len();
    uint8_t data[len];
    ttn_response(data, len);

    char buffer[6];
    for(uint8_t i = 0; i < len; i++){
      Serial.print(F(buffer));
    }
    Serial.println();
  }
}



// -----------------------------------------------------------------------------------------------------------------------------------------------------------
// Funcion para activar o desactivar desde "configuration.h" el monitor serial para debugging
// -----------------------------------------------------------------------------------------------------------------------------------------------------------
void debug_code(){
#if ENABLE_DEBUG == 1
  DEBUG_PORT.begin(SERIAL_BAUD);
#endif
}

// ===========================================================================================================================================================
// Setup main
// ===========================================================================================================================================================
void setup(){
  //
  // Debug ---------------------------------------------------------------------------------------------------------------------------------------------------
  //
  debug_code();

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


  // TTN setup -----------------------------------------------------------------------------------------------------------------------------------------------
  //
  if(!ttn_setup()){
    Serial.println("[ERR] Radio module not found!");
    sleep_forever();
  }

  //
  // TTN register --------------------------------------------------------------------------------------------------------------------------------------------
  //
  ttn_register(callback);                                 // Configuración funcion de eventos Lora
  ttn_join();
  
  ttn_sf(LORAWAN_SF);                                     // Configuración del Spreading Factor
  ttn_adr(LORAWAN_ADR);


  
}

// ===========================================================================================================================================================
// Loop main
// ===========================================================================================================================================================



void loop(){
  
  os_runloop_once();

  unsigned long currentMillis = millis();  // Lee el tiempo actual

  // Verifica si han pasado 30 segundos desde la última vez que se ejecutaron doSensor() y loraSend()
  if (currentMillis - previousMillis >= INTERVAL || firstTime) {
    previousMillis = currentMillis;  // Actualiza el tiempo de la última ejecución
    firstTime =false;
    
    doSensor(txBuffer);
    
    loraSend();
  }
  
}
