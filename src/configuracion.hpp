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

//#define CFG_eu868 1
//#define CFG_sx1276_radio 1

// CREDENTIALS

#define CONFIG_1

#ifdef CONFIG_0

// LoRaWAN NwkSKey, network session key
// This should be in big-endian (aka msb).
static const PROGMEM u1_t NWKSKEY[16] = {0xCE, 0x15, 0x1E, 0x76, 0xA9, 0x44, 0x3C, 0x82, 0x0D, 0x31, 0xE8, 0x5F, 0x7C, 0xF9, 0x75, 0x4B};

// LoRaWAN AppSKey, application session key
// This should also be in big-endian (aka msb).
static const u1_t PROGMEM APPSKEY[16] = {0x08, 0xA5, 0x58, 0x97, 0x60, 0xE2, 0xA6, 0xC9, 0x2A, 0x4B, 0xB1, 0x0C, 0x4B, 0x15, 0x20, 0xE9};

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
// The library converts the address to network byte order as needed, so this should be in big-endian (aka msb) too.
static const u4_t DEVADDR = 0x260BD34E; // <-- Change this address for every node!
#endif

#ifdef CONFIG_1
static const PROGMEM u1_t NWKSKEY[16] = {0xCE, 0x15, 0x1E, 0x76, 0xA9, 0x44, 0x3C, 0x82, 0x0D, 0x31, 0xE8, 0x5F, 0x7C, 0xF9, 0x75, 0x4B};
static const u1_t PROGMEM APPSKEY[16] = {0x08, 0xA5, 0x58, 0x97, 0x60, 0xE2, 0xA6, 0xC9, 0x2A, 0x4B, 0xB1, 0x0C, 0x4B, 0x15, 0x20, 0xE9};
static const u4_t DEVADDR = 0x260BD34E;
#endif

#ifdef CONFIG_2
static const PROGMEM u1_t NWKSKEY[16] = {0xCE, 0x15, 0x1E, 0x76, 0xA9, 0x44, 0x3C, 0x82, 0x0D, 0x31, 0xE8, 0x5F, 0x7C, 0xF9, 0x75, 0x4B};
static const u1_t PROGMEM APPSKEY[16] = {0x08, 0xA5, 0x58, 0x97, 0x60, 0xE2, 0xA6, 0xC9, 0x2A, 0x4B, 0xB1, 0x0C, 0x4B, 0x15, 0x20, 0xE9};
static const u4_t DEVADDR = 0x260BD34E;
#endif

#ifdef CONFIG_OTAA
static const u1_t PROGMEM APPEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};   // LSB
static const u1_t PROGMEM DEVEUI[8] = {0x46, 0xDF, 0x04, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};   // LSB
static const u1_t PROGMEM APPKEY[16] = {0xC0, 0x33, 0x0F, 0xC3, 0xD2, 0x9B, 0xC1, 0x3A, 0xC8, 0x23, 0x98, 0x61, 0xDD, 0x19, 0x82, 0x83};  // MSB
#endif

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 30;