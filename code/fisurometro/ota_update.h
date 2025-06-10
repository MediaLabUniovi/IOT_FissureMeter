#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H

#include <Arduino.h>

void connectWiFi();
String obtenerVersionRemota();
void actualizarFirmware(String firmwareURL, String versionRemota);

#endif
