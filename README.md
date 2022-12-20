# ProFisurometro_STM32
Dispositivo de medida de desplazamientos de tierra y parámetros ambientales con LoRa.

## Estado del proyecto
En el repositorio existen 3 ramas en funcionamiento. Dos de ellas funcionan con OTAA y la otra con ABP. En la siguiente tabla se puede ver un resumen del proyecto. La rama con el encoder de precisión es la última propuesta para el proyecto. En su apartado se explica el funcionamiento general del código, mientras que en resto solo se harán aclaraciones.

| Rama | Protocolo | Encoder |
| ---- | --------- | ------- |
| master | ABP | KY-040 |
| develop-OTAA | OTAA | KY-040 |
| develop-encooder-precsion | OTAA | Encoder de precisión |

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
1. Instalar los drivers para windows. [Tutorial](https://github.com/profezzorn/ProffieOS/wiki/zadig).
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

## Configuración del código
Se ha de configurar la librería de LoRa. Para ello, [se va al archivo de configuración](), donde se comentan todas las líneas salvo las correspondientes a la frecuencia europea:

## Sensores
1. **Sensor de lluvia**. Sensor analógico. Se probó experimentalmente para conseguir una ecuación que relacionara tensión y lluvia alimentandolo a 3.3V. De alimentarlo a 5V habría que realizar este proceso de nuevo. La salida digital no está implementada. Tabla de conexiones:

| Pin Sensor | Pin STM32 |
| ---------- | --------- |
| GND | GND |
| VCC | 3V3 |
| AO | PB0 |
| D0 | NC |

2. **Sensor de humedad del suelo**.
El sensor es parecido al de lluvia y se probó de la misma forma, pero da peor funcionamiento. Existe un [tutorial de calibración](https://www.switchdoc.com/2020/06/tutorial-capacitive-moisture-sensor-grove/). Tabla de conexiones:

| Pin Sensor | Pin STM32 |
| ---------- | --------- |
| GND | GND |
| VCC | 3V3 |
| AU | PB1 |

3. **BME280**.
Es un sensor I2C de temperatura, presión y humedad. Ha de estar algo alejado de cualquier fuente de calor para que la medida sea correcta. Se usa [esta librería] (https://github.com/Seeed-Studio/Grove_BME280). Tabla de conexiones:

| Pin Sensor | Pin STM32 |
| ---------- | --------- |
| GND | GND |
| VCC | 3V3 |
| SCL | PB6 |
| SDA | PB7 |

4. **DS18B20**.
Sensor digital para medir la temperatura del suelo.  Formato de cable. Se puede alimentar tanto a 3.3V como a 5V. Para el conexionado, se coloca una resistencia de 4.7kOhm entre los cables de datos y VCC. Si no se coloca, siempre lee -127ºC (Valor de error). La [librería](https://github.com/milesburton/Arduino-Temperature-Control-Library.git) usada en  otros proyectos con este sensor da problemas con el STM32, por lo que no se ha usado. En su lugar se usa un [archivo](https://github.com/MedialabU/ProFisurometro_STM32/blob/master/src/fnDS18B20.hpp) con funciones específicas para el sensor. Tabla de conexiones:

| Color cable | Función | Pin STM32 |
| ----------- | ------- | --------- |
| Amarillo | DATA | PB5 |
| Rojo | VCC | 3V3 |
| Negro | GND | GND |

5. **KY-040**.
Este fue el dispositivo usado para medir distancia en la primera fase del proyecto. Un pulso de este encoder se traducen en 8.4mm lineales. No es de muy buena calidad y produce rebotes. La función de interruptor al pulsarlo no está implementada. Se puede alimentar tanto a 3.3V como 5V. Tabla de conexiones:

| Pin Sensor | Pin STM32 |
| ---------- | --------- |
| GND | GND |
| + | 5V |
| SW | NC |
| DT | PB9 |
| CLK | PB8 |

6. **Encoder de precisión**.
Dispositivo usado en la fase final del proyecto. No produce rebotes y permite medir distancia con mucha más precisión que el KY-040. Este encoder traduce 1 pulso en 0.08mm de movimiento lineal. Solo se puede alimentar a 5V. La camisa no se conecta. La elongación máxima es de 1m. Se le puede colocar un hilo para extender la distancia y poder poner el dispositivo más lejos del punto fijo, pero no podrá medir distancias mayores a la elongación máxima.

| Color cable | Función | Pin STM32 |
| ----------- | ------- | --------- |
| Camisa | Protección | NC|
| Negro | GND | GND|
| Blanco | Canal A | PB9 |
| Verde | Canal B | PB8 |
| Rojo | VCC | 5V |

## RFM95W
Es el módulo de radio usado para LoRa. Va soldado sobre el [MediaLab RFM95 Shield](https://github.com/MedialabU/Lora-Documentacion). La PCB tiene un lugar específico para colocarlo. El DIO2 se puede dejar sin conectar. Para LoRa se usa esta [librería](https://github.com/mcci-catena/arduino-lmic). Tabla de conexiones:

| Shield | Pin STM32 |
| ---------- | --------- |
| NSS | PB12 |
| SCK | A5 |
| MOSI | A7 |
| MISO | A6 |
| 3.3V | 3V3 |
| G | GND |
| DIO0 | PA0 |
| DIO1 | PA1 |
| DIO2 | NC |
| RESET | PA3 |


