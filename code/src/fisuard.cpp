//fisuard.cpp
#include "configuration.h"
#include "sensors.h"

#define pinMOSI PA7
#define pinMISO PA6
#define pinSCLK PA5

STM32RTC& rtc = STM32RTC::getInstance();
TwoWire myWire(PB7, PB6); // SDA, SCL
Adafruit_BME280  bme280;

static uint32_t atime = 1800000;
volatile bool alarmTriggered = false;        // Bandera para indicar que la alarma ha despertado el micro
volatile bool Encoder = true; // Variable que regula las interrupciones del encoder

//Mapa de pines del chip lora
const lmic_pinmap lmic_pins = {
    .nss = PB12,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = PA3,
    .dio = {PA0, PA1, LMIC_UNUSED_PIN},
};

//Credenciales de TTN
void os_getDevEui (u1_t* buf) {
    // Copia tu Device EUI aquí en orden inverso
    memcpy_P(buf, "\xAE\x7A\x06\xD0\x7E\xD5\xB3\x70", 8);
}

void os_getArtEui (u1_t* buf) {
    // Copia tu Application EUI aquí en orden inverso
    memcpy_P(buf, "\x00\x00\x00\x00\x00\x00\x00\x00", 8);
}

void os_getDevKey (u1_t* buf) {
    // Copia tu App Key aquí
    
    memcpy_P(buf, "\x43\x68\x46\xEC\x1B\x6A\x89\xED\x3B\xEB\xEB\x12\x7C\xBF\x50\xB8", 16);
}

void LoRaSend(){

  //Al despertar el micro se tienen que volver a realizar todas las configuraciones del LoRa
  LMIC_reset();
  LMIC_startJoining();

  // Esperar hasta que el dispositivo se una a la red LoRaWAN
  while (LMIC.opmode & OP_JOINING) {
        os_runloop_once();  // Ejecuta el bucle de eventos para manejar la unión
    }

}

//Eventos de LoRa
void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    Serial1.println("Evento LoRa:");
    Serial1.println(ev);
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));
            // Desactiva la verificación de enlace ya que no necesitamos confirmación
            LMIC_setLinkCheckMode(0);
            // Enviar un paquete inmediatamente después de unirse
            do_send();
            break;

    }
}

/* Callback de la alarma */
void alarmMatch(void* data) {

  Encoder = false; // Desactiva la interrupción del encoder
  alarmTriggered = true; // Cambia el valor del flag cuando se despierta por la alarma

}

void setup() {

  //Serial.begin(115200);
  //Serial.println(F("Starting"));
  //Serial1.begin(19200);

  //pinMode(LED_BUILTIN, OUTPUT);

  //Configuración del RTC
  #if defined(RTC_BINARY_NONE)
    rtc.setClockSource(STM32RTC::LSE_CLOCK);
    rtc.begin(true); /* reset the RTC else the binary mode is not changed */
  #else
    rtc.begin();
  #endif /* RTC_BINARY_NONE */

  if(bme280.begin(0x76, &myWire)){
    Serial.println("BME280 Iniciado");
  }

  bme280.setSampling(Adafruit_BME280::MODE_FORCED);

  // Configure low power
  LowPower.begin();
  LowPower.enableWakeupFrom(&rtc, alarmMatch, &atime);

  delay(1000);

  // Configurar las interrupciones del encoder
  pinMode(pinEncoderClk, INPUT_PULLUP);
  pinMode(pinEncoderData, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinEncoderClk), encoderA, CHANGE); // Interrupción en cambios
  attachInterrupt(digitalPinToInterrupt(pinEncoderData), encoderB, CHANGE); // Interrupción en cambios

  //Se toman medidas de los sensores una primera vez
  doSensors();
    
  //Serial1.println("Start");

  // Configuraciones del LoRa
  os_init();
  LMIC_reset();
  LMIC_startJoining();


  // Configure first alarm in 2 second then it will be done in the rtc callback
  rtc.setAlarmEpoch(rtc.getEpoch() + 2);

}

void loop() {

if (alarmTriggered) {

    //Serial1.println("Me despierto");
    //digitalWrite(LED_BUILTIN, LOW);
    
    // Ejecuta las funciones necesarias al despertar
    doSensors();


    LoRaSend();

    // Configura la próxima alarma en `atime` segundos para que sea recurrente
    rtc.setAlarmEpoch(rtc.getEpoch() + atime / 1000);
    Encoder = true;
    alarmTriggered = false;  // Reinicia la bandera
  }
  //digitalWrite(LED_BUILTIN, HIGH);

  LowPower.deepSleep(); // Entra en modo STOP (deepSleep) hasta que la alarma lo despierte

}



