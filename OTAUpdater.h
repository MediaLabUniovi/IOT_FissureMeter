#ifndef OTA_UPDATER_H
#define OTA_UPDATER_H

#include <Arduino.h>

void OTA_begin();
void OTA_check(); 
void OTA_forceUpdate(); // Para usar con botón o condición especial

#endif