
#include "configuration.h"


#define pinMOSI PA7
#define pinMISO PA6
#define pinSCLK PA5


TwoWire myWire(PB7, PB6); // SDA, SCL
Adafruit_BME280  bme280;

unsigned long lastSensorRead = 0;  // Último tiempo en que se ejecutó doSensors
const unsigned long sensorInterval = 25000;  // Intervalo para doSensors (20 segundos = 20000 milisegundos)


const lmic_pinmap lmic_pins = {
    .nss = PB12,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = PA3,
    .dio = {PA0, PA1, LMIC_UNUSED_PIN},
};


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



void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    Serial.println(ev);
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
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.println(F("Received "));
              Serial.println(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            // Esperar un tiempo antes de enviar el siguiente mensaje
            delay(30000); // Espera 10 segundos antes de enviar el siguiente mensaje
            do_send(); // Enviar el siguiente mensaje
            break;
        // Agrega otros eventos según sea necesario
    }
}



void setup() {
    Serial.begin(115200);
    Serial.println(F("Starting"));
    //Wire.begin(PB7, PB6);
    //scanI2CAddresses();

    if(bme280.begin(0x76, &myWire)){
      Serial.println("BME280 Iniciado");
    }

    delay(1000);
  
    pinMode(pinEncoderClk, INPUT_PULLUP);
    pinMode(pinEncoderData, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pinEncoderClk), encoderA, CHANGE); // Interrupción en cambios
    attachInterrupt(digitalPinToInterrupt(pinEncoderData), encoderB, CHANGE); // Interrupción en cambios


    doSensors();
    
    Serial.println("Start");
    os_init();
    LMIC_reset();
    LMIC_startJoining();
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Verifica si ha pasado suficiente tiempo desde la última lectura de sensores
  if (currentMillis - lastSensorRead >= sensorInterval) {
    doSensors();  // Ejecuta la función doSensors
    lastSensorRead = currentMillis;  // Actualiza el último tiempo de ejecución
  }
  os_runloop_once();
}

void scanI2CAddresses() {
    byte error, address;
    int nDevices;

    Serial.println("Escaneando dispositivos I2C...");

    nDevices = 0;
    for(address = 1; address < 127; address++ ) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("Dispositivo I2C encontrado en la dirección 0x");
            if (address < 16) 
                Serial.print("0");
            Serial.println(address, HEX);
            nDevices++;
        }
        else if (error == 4) {
            Serial.print("Error desconocido en la dirección 0x");
            if (address < 16) 
                Serial.print("0");
            Serial.println(address, HEX);
        }    
    }
    if (nDevices == 0)
        Serial.println("No se encontraron dispositivos I2C.");
    else
        Serial.println("Escaneo I2C completado.");
}

