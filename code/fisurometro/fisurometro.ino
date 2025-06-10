/* ***********************************************************************************************************************************************************
Lora to Do - MEDIALAB LPWAN UNIVERSIDAD DE OVIEDO
*********************************************************************************************************************************************************** */
// ==========================
//        INCLUDES
// ==========================
#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#include <OneWire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_SSD1306.h>
#include <DallasTemperature.h>

#include "configuration.h"
#include "credentials.h"
#include "lmic_project_config.h"
#include "ota_update.h"
#include "rom/rtc.h"

// ==========================
//     DEFINICIONES FIRMWARE
// ==========================
const char* FIRMWARE_VERSION = "1.0.1";

// ==========================
//     VARIABLES GLOBALES
// ==========================
static uint8_t txBuffer[TX_BUFFER_SIZE];  // Buffer de envío LoRa
unsigned long previousMillis = 0;         // Para control de tiempo de envío
unsigned long previousOtaMillis = 0;      // Para control de tiempo de OTA
const unsigned long OTA_INTERVAL = 2 * 60 * 60 * 1000;  // Intervalo OTA (2 horas)
boolean firstTime = true;                 // Bandera para primer envío

// ==========================
//        SENSORES
// ==========================
Adafruit_BMP280 bme280;                      // Sensor de temperatura y presión
OneWire oneWireTemp(PIN_DS18);              // Comunicación con sensor DS18B20
DallasTemperature dTemp(&oneWireTemp);      // Sensor de temperatura de suelo

// ==========================
//     MEMORIA NO VOLÁTIL
// ==========================
Preferences preferences;
extern int counter;

// ==========================
//      BOTÓN DE RESET
// ==========================
unsigned long tiempoPresionado = 0;
unsigned long tiempoLiberado = 0;
const unsigned long umbralLargo = 7000;
bool botonEstado = false;
bool ultimoBotonEstado = false;
unsigned long tiempoAccion = 0;
bool esperandoReinicio = false;

// ==========================
//        PANTALLA OLED
// ==========================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



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
// Funcion para dormir el ESP32 durante el tiempo de duty cycle (NO SE UTILIZA)
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
  
  //Recuperación de último valor de encoder guardado
  preferences.begin("my_data", false);
  counter = preferences.getInt("encoder", 0);  // 0 si no existe
  preferences.end();
  Serial.print("Valor de encoder recuperado: ");
  Serial.println(counter);

  btStop();  // Desactiva Bluetooth por completo al arrancar
  pinMode(PIN_RESET, INPUT_PULLUP);  //RESET
  connectWiFi();
  delay(100);

  if (bme280.begin(0x76)) {
    Serial.println("BMP280 Iniciado");
  }
  delay(100);
  //Comprobación Firmware
  if (WiFi.status() == WL_CONNECTED) {

    String versionRemota = obtenerVersionRemota();

    if (versionRemota != "" && versionRemota != String(FIRMWARE_VERSION)) {
      Serial.println("Nueva versión detectada. Actualizando...");
      actualizarFirmware(firmwareURL, versionRemota);  // ✅ Pasamos la versión como argumento
    } else {
      Serial.println("Sin cambios de versión.");
    }
  }

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

// -----------------------------------------------------------------------------------------------------------------------------------------------------------
// Uso de pantalla de LilyGo
// -----------------------------------------------------------------------------------------------------------------------------------------------------------


  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Fallo al iniciar pantalla OLED"));
    while (true);
  }

  // Limpiar buffer
  display.clearDisplay();

  // Configurar texto
  display.setTextSize(2);       // Tamaño del texto
  display.setTextColor(SSD1306_WHITE);  // Color del texto
  display.setCursor(10, 25);    // Posición (x, y)

    // Mostrar texto
  display.println("TERRANATOR");
  display.display();  // Renderizar
  
}

// ===========================================================================================================================================================
// Loop main
// ===========================================================================================================================================================

void loop() {
  // ==========================
  //   RUN DEL STACK LoRa
  // ==========================
  os_runloop_once();

  // ==========================
  //    LECTURA DEL BOTÓN
  // ==========================
  botonEstado = digitalRead(PIN_RESET) == LOW;

  if (botonEstado != ultimoBotonEstado) {
    if (botonEstado) {
      tiempoPresionado = millis();
    } else {
      tiempoLiberado = millis();
      unsigned long duracion = tiempoLiberado - tiempoPresionado;

      if (duracion >= umbralLargo) {
        Serial.println("Pulsación larga detectada");

        // Resetear encoder a 0 antes de reiniciar
        preferences.begin("my_data", false);
        preferences.putInt("encoder", 0);
        preferences.end();
        Serial.println("Valor de encoder 0");

        tiempoAccion = millis();
        esperandoReinicio = true;

      } else {
        Serial.println("Pulsación corta detectada");

        // Guardar valor actual del encoder
        preferences.begin("my_data", false);
        preferences.putInt("encoder", counter);
        preferences.end();

        tiempoAccion = millis();
        esperandoReinicio = true;
      }
    }
    ultimoBotonEstado = botonEstado;
  }

  // ==========================
  //     ANTI-REBOTE (DEBOUNCE)
  // ==========================
  static unsigned long lastDebounceTime = 0;
  if (millis() - lastDebounceTime > 50) {
    lastDebounceTime = millis();
  }

  // ==========================
  //     ESPERAR Y REINICIAR
  // ==========================
  if (esperandoReinicio && millis() - tiempoAccion >= 300) {
    esp_restart();
  }

  // ==========================
  //   ENVÍO PERIÓDICO LORA
  // ==========================
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= INTERVAL || firstTime) {
    previousMillis = currentMillis;
    firstTime = false;

    doSensor(txBuffer);  // Leer sensores y llenar buffer
    loraSend();          // Enviar datos por LoRa
  }

  // ==========================
  //     ACTUALIZACIÓN OTA
  // ==========================
  if (currentMillis - previousOtaMillis >= OTA_INTERVAL) {
    previousOtaMillis = currentMillis;

    if (WiFi.status() == WL_CONNECTED) {
      String versionRemota = obtenerVersionRemota();

      if (versionRemota != "" && versionRemota != String(FIRMWARE_VERSION)) {
        Serial.println("Nueva versión detectada. Actualizando...");
        actualizarFirmware(firmwareURL, versionRemota);
      } else {
        Serial.println("Sin cambios de versión.");
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        Serial.println("WiFi apagado para reducir consumo");
      }

    } else {
      Serial.println("WiFi no conectado, intentando reconectar...");
      connectWiFi();
    }
  }
}

