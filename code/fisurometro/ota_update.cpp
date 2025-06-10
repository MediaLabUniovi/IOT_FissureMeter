// ==========================
//         INCLUDES
// ==========================
#include "ota_update.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPUpdate.h>
#include <HTTPClient.h>

// ==========================
//   DEFINICIÓN DE RED WIFI
// ==========================
#define WIFI_SSID     "movil"
#define WIFI_PASSWORD "monitor1"

// ==========================
//     URL DE VERSIÓN OTA
// ==========================
const char* version_url = "https://raw.githubusercontent.com/iyanIM/pruebasOTA/main/version.txt";

// ==========================
//      CONEXIÓN A WIFI
// ==========================
void connectWiFi() {
  Serial.print("Conectando a ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 10) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("No se pudo conectar al WiFi. Continuando sin conexión.");
    WiFi.disconnect(true);  // Apaga el WiFi
    WiFi.mode(WIFI_OFF);    // Desactiva completamente la interfaz WiFi
    Serial.println("WiFi apagado para reducir consumo");
  }
}

// ==========================
// OBTENER VERSIÓN REMOTA OTA
// ==========================
String obtenerVersionRemota() {
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, version_url);

  String nuevaVersion = "";
  int httpCode = http.GET();

  if (httpCode == 200) {
    nuevaVersion = http.getString();
    nuevaVersion.trim();
    Serial.print("Versión remota: ");
    Serial.println(nuevaVersion);
  } else {
    Serial.printf("Error al obtener version.txt: %d\n", httpCode);
  }

  http.end();
  return nuevaVersion;
}

// ==========================
//  ACTUALIZACIÓN DEL FIRMWARE
// ==========================
void actualizarFirmware(String firmwareURL, String versionRemota) {
  WiFiClientSecure client;
  client.setInsecure();

  Serial.println("Buscando actualización OTA...");

  String firmware = firmwareURL + versionRemota + ".bin";

  t_httpUpdate_return resultado = httpUpdate.update(client, firmware);

  switch (resultado) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("Actualización fallida. Error: %s\n", httpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("No hay actualizaciones disponibles.");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("Actualización OTA completada.");
      break;
  }
}
