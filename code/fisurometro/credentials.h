#pragma once

#define USE_OTAA

#ifdef USE_OTAA

    // This EUI must be in little-endian format, so least-significant-byte
    // first. When copying an EUI from ttnctl output, this means to reverse
    // the bytes. For TTN issued EUIs the last bytes should be 0x00, 0x00,
    // 0x00.
    static const u1_t PROGMEM APPEUI[8]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  // INTRODUCIR EN LSB

    // This should also be in little endian format, see above. (least-significant-byte
    // first)
    static const u1_t PROGMEM DEVEUI[8]  = {0xAE, 0x7A, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};  // INTRODUCIR EN LSB

    // This key should be in big endian format (or, since it is not really a
    // number but a block of memory, endianness does not really apply). In
    // practice, a key taken from ttnctl can be copied as-is.
    // The key shown here is the semtech default key.
    static const u1_t PROGMEM APPKEY[16] = {0x43, 0x68, 0x46, 0xEC, 0x1B, 0x6A, 0x89, 0xED, 0x3B, 0xEB, 0xEB, 0x12, 0x7C, 0xBF, 0x50, 0xB8};  // INTRODUCIR EN MSB

#endif
