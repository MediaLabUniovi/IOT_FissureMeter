#include "OTAUpdater.h"
#include "config.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>

unsigned long lastOTACheck = 0;

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.printf("Conectando a %s", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
}

String getRemoteVersion() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, VERSION_URL);

  String version = "";
  if (http.GET() == 200) {
    version = http.getString();
    version.trim();
  } else {
    Serial.println("Error al obtener la versión remota.");
  }
  http.end();
  return version;
}

void performUpdate(const String& version) {
  WiFiClientSecure client;
  client.setInsecure();

  String fullUrl = String(FIRMWARE_BASE_URL) + version + ".bin";
  t_httpUpdate_return ret = httpUpdate.update(client, fullUrl);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("Error en la actualización OTA: %s\n", httpUpdate.getLastErrorString().c_str());
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("No hay actualizaciones disponibles.");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("Actualización completada.");
      break;
  }
}

void OTA_forceUpdate() {
  connectWiFi();
  if (WiFi.status() != WL_CONNECTED) return;

  String remoteVersion = getRemoteVersion();
  if (remoteVersion != "" && remoteVersion != FIRMWARE_VERSION) {
    Serial.println("Nueva versión detectada. Actualizando...");
    performUpdate(remoteVersion);
  } else {
    Serial.println("Firmware actualizado. No se necesita actualización.");
  }
}

void OTA_begin() {
  OTA_forceUpdate();
}

void OTA_check() {
  unsigned long current = millis();
  if (current - lastOTACheck >= OTA_CHECK_INTERVAL) {
    lastOTACheck = current;
    OTA_forceUpdate();
  }
}