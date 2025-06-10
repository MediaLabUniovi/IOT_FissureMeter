# 🔄 OTA Updater para ESP32

Este proyecto demuestra cómo implementar actualizaciones OTA (Over The Air) automáticas y controladas en un ESP32. Está diseñado para ser modular y 
fácilmente integrable en cualquier otro proyecto.

---

## 🚀 ¿Qué hace este proyecto?

- Conecta a WiFi.
- Verifica si hay una nueva versión disponible en un archivo remoto `version.txt`. 
-El firmware subido a github tiene que seguir la siguiente estructura de nombre "firmware-v1.0.x.bin" (se cambia la x por un número)
- Si encuentra una nueva versión, descarga y actualiza automáticamente el firmware.
- Se puede forzar la verificación OTA pulsando un botón (ej. botón RESET).
- Apaga WiFi tras la verificación para ahorrar energía (opcional).

---

## 📁 Estructura del Proyecto

