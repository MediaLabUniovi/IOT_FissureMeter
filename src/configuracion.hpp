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
#define DEEP_SLEEP
//#define NO_SLEEP
//#define LIGHT_SLEEP

// CREDENTIALS
#define CONFIG_0

#ifdef CONFIG_0     // fisurometro/devices/pro-fisurometro
static const PROGMEM u1_t NWKSKEY[16] = {0xD6, 0x8F, 0xE0, 0x93, 0x6C, 0x7F, 0x4B, 0xAF, 0x30, 0x4B, 0xB9, 0x15, 0x51, 0xC8, 0x9A, 0x62};
static const u1_t PROGMEM APPSKEY[16] = {0xE0, 0x09, 0x9E, 0x01, 0xE6, 0x47, 0xF3, 0x19, 0x7A, 0x2C, 0xDF, 0x97, 0xA9, 0x29, 0x28, 0x8D};
static const u4_t DEVADDR = 0x260BA2AA; 
#endif


// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 30;