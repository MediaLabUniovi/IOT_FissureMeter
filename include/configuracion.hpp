#include <Arduino.h>
#include <lmic.h>

#define _LMIC_CONFIG_PRECONDITIONS_H_

#define SERIAL_BAUD 115200         // Serial debug baud rate
#define SLEEP_BETWEEN_MESSAGES 1   // Do sleep between messages
#define MAX_SLEEP_INTERVAL 30000   // Sleep for these many millis
#define SLEEP_DELAY 500            // Time between sleep blocks to keep IP5306 on
#define MESSAGE_TO_SLEEP_DELAY 500 // Time after message before going to sleep
#define LORAWAN_PORT 1             // Port the messages will be sent to
#define LORAWAN_CONFIRMED_EVERY 0  // Send confirmed message every these many messages (0 means never)
#define LORAWAN_SF DR_SF7          // Spreading factor
#define LORAWAN_POW 14             // Transmitting power in dBm (0 to 14 for EU bands)
#define LORAWAN_ADR 0              // Enable ADR


// Modos de bajo consumo
//#define DEEP_SLEEP
#define NO_SLEEP
//#define LIGHT_SLEEP

// CREDENTIALS
#define CONFIG_0

#ifdef CONFIG_0     // fisurometro/devices/pro-fisurometro
static const PROGMEM u1_t NWKSKEY[16] = {0x4C, 0x75, 0x22, 0x75, 0x03, 0xDB, 0x0B, 0x47, 0x84, 0xBD, 0x8E, 0xA2, 0xCE, 0x99, 0xD8, 0xB8};
static const u1_t PROGMEM APPSKEY[16] = {0x5C, 0xDA, 0xC4, 0xAB, 0x53, 0xB7, 0x42, 0x46, 0x3F, 0x20, 0xD9, 0x3A, 0xBE, 0x70, 0x6C, 0x63};
static const u4_t DEVADDR = 0x260B892C; 
#endif


// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 30;