# ProFisurometro_STM32
Dispositivo de medida de desplazamientos de tierra y parámetros ambientales con LoRa.

## Estado del proyecto
En el repositorio existen 3 ramas en funcionamiento. Dos de ellas funcionan con OTAA y la otra con ABP. En la siguiente tabla se puede ver un resumen del proyecto. La rama con el encoder de precisión es la última propuesta para el proyecto. En su apartado se explica el funcionamiento general del código, mientras que en resto solo se harán aclaraciones.

| Rama | Protocolo | Encoder |
| ---- | --------- | ------- |
| master | ABP | KY-040 |
| develop-OTAA | OTAA | KY-040 |
| develop-encooder-precsion | OTAA | Encoder de precisión |

Para las ramas que no llevan el encoder de precisión se usa una función de lectura del encoder especial, que hace la adquisición de datos más lenta pero más robusta. elimina en gran medida rebotes e imprecisiones. El funcionamiento es prácticamente predictivo, usa una lista de valores para comprobar si el dato que acaba de leer se corresponde con lo que debería de haber leído si el encoder fuese perfecto.

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
La librería *LoRaWAN*  utilizada es [arduino-limc](https://github.com/mcci-catena/arduino-lmic). Se ha de configurar para frecuencias europeas. Para ello, se va al [archivo de configuración](https://github.com/mcci-catena/arduino-lmic/blob/master/project_config/lmic_project_config.h), donde se comentan todas las líneas salvo las correspondientes a la frecuencia europea:

```c++
// project-specific definitions
#define CFG_eu868 1
// #define CFG_us915 1
//#define CFG_au915 1
//#define CFG_as923 1
// #define LMIC_COUNTRY_CODE LMIC_COUNTRY_CODE_JP      /* for as923-JP; also define CFG_as923 */
//#define CFG_kr920 1
//#define CFG_in866 1
#define CFG_sx1276_radio 1
//#define LMIC_USE_INTERRUPTS
```
Ruta del archivo en proyecto de PlatformIO: ***\.pio\libdeps\blackpill_f411ce\MCCI LoRaWAN LMIC library\project_config\lmic_project_config.h***

## Explicación del código
### 1. Archivo de configuración
El archivo contiene líneas de configuración general para el proyecto y la librería LMIC.

#### 1.1 Modos de bajo consumo
Existen tres macros para configurar los modos de bajo consumo. Solo se descomenta una. Con el modo ***DEEP_SLEEP*** se consigue el funcionamiento deseado, pero no permite la utilización del serial. El modo ***NO_SLEEP*** provoca que los datos se envíen cada intervalo de tiempo **TX_INTERVAL**, en vez de cada vez que se detecte un cambio en el encoder, haciendo que el microcontrolador nunca entre en modo de bajo consumo. El modo ***LIGHT_SLEEP*** permite interactuar con el serial pero el microcontrolador consume demadsiada corriente cuando no está despierto. [Librería](https://github.com/stm32duino/STM32LowPower) de modos de bajo consumo.

#### 1.2 Credenciales
Solo puede estar definido un conjunto de credenciales. Es importante prestar atención a la endianness. (MSB first o LSB first). Las credenciales de OTAA para el end device ID test-branch-celialon son:
``` c++
static const u1_t PROGMEM APPEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};   // LSB first
static const u1_t PROGMEM DEVEUI[8] = {0xCF, 0x1A, 0x05, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};   // LSB first
static const u1_t PROGMEM APPKEY[16] = {0xCE, 0x32, 0x89, 0x84, 0x6F, 0x32, 0xA2, 0xB1, 0xB2, 0xDA, 0x6C, 0xC8, 0x37, 0xDA, 0x07, 0x57};  // MSB first
```

### 2 Archivo de pines
El archivo contiene definiciones para interactuar con los pines del Black Pill.

### 3. Librería del DS18B20
La [librería](https://github.com/milesburton/Arduino-Temperature-Control-Library.git) utilizada para este sensor en otros proyectos con microcontroladores Arduino dio problemas con el STM32, , por lo que el [archivo](https://github.com/MedialabU/ProFisurometro_STM32/blob/develop-encoder-precisi%C3%B3n/src/fnDS18B20.hpp) contiene las funciones necesarias para obtener la temperatura. Se las llama desde el archivo principal.

### 4. Archivo principal
Aquí está la lectura de los sensores y el envío de datos por LoRa. El mapa de pines es la forma de decirle a la librería ***arduino-lmic*** donde está conectado el ***RFM95W***. Es espéfifico de cada microcontrolador.

```c++
const lmic_pinmap lmic_pins = {
    .nss = PB12,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = PA3,
    .dio = {PA0, PA1, LMIC_UNUSED_PIN},
};
```
La lectura de los sensores y la construcción del array a enviar se hace dentro de la función de envío de datos. Los tipos *float* se convierten a enteros haciendo un escalado binario. La presión se divide entre 100 ya que es un valor muy grande y excede la capacidad del tipo *int.* Esto no afecta ya que generalmente la presión atmosférica se suele dar del orden de los hectopascales.

Una vez convertidos a enteros, se separan en parte alta y parte baja y se almacenan en el array. Los pulsos del encoder también es un valor muy grande pero en este caso se desea mantener la máxima precisión, por lo que se utiliza el tipo estándar de 32 bits para almacenarlo y se separa en 4 bytes para enviar.

Una vez construido el array, se prepara para enviar lo antes posible.

En la parte de gestión de eventos se hace la llamada a la función de envío y manda a modo de bajo consumo.

En el *setup* se inicializan las librerías. Se configuran las interrupciones en función del modo de bajo consumo seleccionado. El STM32 cuenta con varios puertos, por lo que es importante configurar los pines de comunicación digital que se van a utilizar. esto no es necesario en la mayoría de microcontroladores Arduino. Al finalizar la configuración se envía el primer conjunto de datos y se manda al modo de bajo consumo correspondiente.

El *loop* solo ejecuta la función del ciclo de la librería LMIC.

### 5. Función para el encoder ***KY-040***
```c++
void read_encoder() 
{
  static uint8_t prevClkData = 3;  // Lookup table index
  static int8_t encVal = 0;   // Encoder value  
  static const int8_t enc_states[]  = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0}; // Lookup table

  prevClkData <<=2;  // Remember previous state

  if (digitalRead(pinCLK)) prevClkData |= 0x02; // Add current state of pin A
  if (digitalRead(pinDATA)) prevClkData |= 0x01; // Add current state of pin B
  
  encVal += enc_states[( prevClkData & 0x0f )];

  // Update counter if encoder has rotated a full indent, that is at least 4 steps
  if( encVal > 3 ) {        // Four steps forward
    counter++;              // Increase counter
    encVal = 0;
  }
  else if( encVal < -3 ) {  // Four steps backwards
   counter--;               // Decrease counter
   encVal = 0;
  }
}
```
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


