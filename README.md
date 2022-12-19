# ProFisurometro_STM32

## Componentes
| Parte  | Componente | Enlace de compra |
| ------------- | ------------- | -------|
| Microcontrolador  | Black Pill v2.0 (STM32411CE)  | [amazon](https://www.amazon.com/-/es/CANADUINO-STM32-Pastilla-STM32F411CEU6-incluido/dp/B0847G8RQF)|
| Medidor de distancia  |  Encoder rotativo de precisión. (Modelo 1) | [aliexpress](https://es.aliexpress.com/item/4000354494107.html) |
| Sensor de temperatura/presión/humedad ambiente |  BME280 | [amazon](https://www.amazon.es/TECNOIOT-Temperatura-tuberías-barométrico-gy-bme280/dp/B07RLM9D3S) |
| Sensor de lluvia  |  AZ Delivery Regen Sensor | [amazon](https://www.amazon.es/AZDelivery-Lluvia-lluvia-Sensor-Arduino/dp/B07CP2GX9P/ref=sr_1_5) |
| Sensor de humedad del suelo | Capacitive Soil Moisture Sensor | [amazon](https://www.amazon.es/KeeYees-Piezas-Módulo-Humedad-Capacitivo/dp/B07R174TM1/ref=sr_1_6) |
| Sensor de temperatura del suelo  | DS18B20 | [amazon](https://www.amazon.es/ICQUANZX-Temperatura-Termómetro-termopar-Resistencia/dp/B088LQ7HMH/ref=sr_1_8) |
| Módulo de radio para LoRa | RFM95W sobre MediaLab Shield | [amazon](https://www.amazon.es/BOBEINI-Transceptor-inalámbrico-LoRaTM-transmisor-receptor/dp/B09DS8BZ3D/ref=sr_1_7) |

## Uso y configuración de la Black Pill con Windows y PlatformIO
1. Instalar los drivers para windows. [Tutorial](https://github.com/profezzorn/ProffieOS/wiki/zadig)
2. Configuración del archivo .ini para poder programar el microcontrolador a través del puerto USB:
``` ini
; PlatformIO Project Configuration File
;
;   Build options: build flags, source filter
;   Upload options: custom upload port, speed and extra flags
;   Library options: dependencies, extra library storages
;   Advanced options: extra scripting
;
; Please visit documentation for the other options and examples
; https://docs.platformio.org/page/projectconf.html

[env:blackpill_f411ce]
platform = ststm32
board = blackpill_f411ce
framework = arduino

upload_protocol = dfu
upload_port = anything
build_flags = 
	-D PIO_FRAMEWORK_ARDUINO_ENABLE_CDC
	-D USBCON
monitor_dtr = 1
monitor_speed = 115200
```
3. Para poder subir el proyecto al microcontolador se ha de poner en modo *BOOTLOADER* de forma manual. Para ello se ha depulsar una vez el botón *NRST* mientras se mantiene pulsado el botón *BOOT*.
4. Cargar el proyecto en el microcontrolador.

## Sensores
