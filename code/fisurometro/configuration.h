#pragma once

// ==========================
//        INCLUDES
// ==========================
#include <Arduino.h>
#include <Wire.h>
#include <lmic.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <OneWire.h>
#include <DallasTemperature.h>

// ==========================
//   DEFINICIONES GLOBALES
// ==========================
#define TX_BUFFER_SIZE          18         // Tamaño del buffer de transmisión LoRa
#define CLOCK_ERROR             1
#define DEBUG_PORT              Serial     // Puerto serial para debug
#define SERIAL_BAUD             115200     // Baudrate del puerto serial
#define ENABLE_DEBUG            1          // Habilitar mensajes de debug
#define SLEEP_BETWEEN_MESSAGES  0          // Control del modo sueño entre mensajes
#define SEND_INTERVAL           10000      // Tiempo de sueño (en ms) entre envíos (si aplica)
#define INTERVAL                30000      // Tiempo entre mensajes si no se duerme
#define LORAWAN_PORT            1          // Puerto LoRaWAN usado para transmisión
#define LORAWAN_CONFIRMED_EVERY 0          // Cada cuántos mensajes se envía uno confirmado
#define LORAWAN_SF              DR_SF7     // Factor de expansión (spreading factor)
#define LORAWAN_ADR             1          // Habilitar ADR (adaptative data rate)

// ==========================
//    URL DE FIRMWARE OTA
// ==========================
String firmwareURL = "https://raw.githubusercontent.com/iyanIM/pruebasOTA/main/firmware-v";

// ==========================
//        DEBUG MACROS
// ==========================
#ifdef DEBUG_PORT
  #define DEBUG_MSG(...) DEBUG_PORT.printf(__VA_ARGS__)
#else
  #define DEBUG_MSG(...)
#endif

// ==========================
//    EVENTOS PERSONALIZADOS
// ==========================
#define EV_QUEUED       100
#define EV_PENDING      101
#define EV_ACK          102
#define EV_RESPONSE     103

// ==========================
//     PINES DE SENSORES
// ==========================
#define PIN_DS18    23    // Sensor de temperatura DS18B20
#define PIN_HUM     36    // Sensor de humedad (ej. capacitivo)
#define PIN_LLUVIA  39    // Sensor de lluvia
#define PIN_BAT     34    // Medición de batería
#define BME_SCL     22    // I2C SCL para BME280
#define BME_SDA     21    // I2C SDA para BME280

// ==========================
//   PINES DEL ENCODER
// ==========================
#define PIN_EN_CLK  13
#define PIN_EN_DT   12
#define PIN_EN_SW   15

// ==========================
//     PIN DEL BOTÓN RESET
// ==========================
#define PIN_RESET   2

// ==========================
//     CONFIG SPI LoRa
// ==========================
#define SCK_GPIO    5
#define MISO_GPIO   19
#define MOSI_GPIO   27
#define NSS_GPIO    18
#define RESET_GPIO  14
#define DIO0_GPIO   26
#define DIO1_GPIO   33
#define DIO2_GPIO   32

// ==========================
// CONTROL DE POTENCIA LoRa
// ==========================
#define LORA_POWER_CTRL_CH 2  // Canal de control de energía (AXP192, T-Beam Rev1)
