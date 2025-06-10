#include "configuration.h"

float temp;
float hum;
float press;
float dallasTemp;
int counter = 0;


void getBME(){

   Serial.print("Temperatura = ");
   temp = bme280.readTemperature();
   Serial.print(temp);
   Serial.println(" °C");

   Serial.print("Presión = ");
   press = bme280.readPressure() /100.0;
   Serial.print(press);
   Serial.println(" hPa");
}

//sensor de temperatura de suelo
void getDallasTemp(){       
    dTemp.begin();
    dTemp.requestTemperatures(); // Solicita las temperaturas a los sensores
    dallasTemp = dTemp.getTempCByIndex(0); // Lee la temperatura del primer sensor encontrado en el bus de oneWire

    if (dallasTemp == DEVICE_DISCONNECTED_C) {
        Serial.println("Error: Sensor desconectado"); // Verifica si el sensor está desconectado
    } else if (dallasTemp == 85.0) {
        Serial.println("Temperatura suelo: dato ignorado"); // Se invalida la lectura de 85 porque a veces surge sin explicación alguna
    } else {
        Serial.print("Temperatura suelo: ");
        Serial.print(dallasTemp);
        Serial.println(" °C");
    }
}

// Lectura analógica del sensor de humedad que se entierra
int getHum() {
  int humS = analogRead(PIN_HUM);  

  const int AirValue = 3600;     // Valor para suelo seco
  const int WaterValue = 0;      // Valor para suelo mojado

  // Mapear el valor a porcentaje 
  int humPercent = map(humS, AirValue, WaterValue, 0, 100);

  // Limitar el valor entre 0% y 100%
  humPercent = constrain(humPercent, 0, 100);

  //Serial.print("Humedad suelo (raw): ");
  //Serial.println(humS);
  Serial.print("Humedad suelo (%): ");
  Serial.println(humPercent);

  return humPercent;
}

//Sensor de lluvia
uint16_t getRain() {        
  int rain = analogRead(PIN_LLUVIA);  // Lectura analógica del sensor

  // Reducción de la resolución a 10 bits (desplazando los 2 bits más significativos) porque la caracterización del sensor se realizó desde 0 a 1023 valores de lectura
  rain = rain >> 2; // Esto reduce la resolución de 12 a 10 bits

  Serial.print("Valor analógico de lluvia: ");
  Serial.println(rain);

    // Calcular mililitros aproximados con la caracterización descrita en notion
    float ml = (582.31 - rain) / 1.3873;

    // Limitar el valor de ml para evitar valores negativos si rain > 582
    if (ml < 0) ml = 0;

  Serial.print("Cantidad aproximada de agua (ml): ");
  Serial.println(ml, 1); // 1 decimal de precisión

  return rain;
}

//Lectura analógica con divisor de tensiones (2 resistencias de 10k) 
uint8_t getBatt() {
  int raw = analogRead(PIN_BAT)*2 - 400; //se le resta el offset obtenido entre el valor real y el medido, y se multiplica por 2 por el divisor de tensiones

  // Convertimos y limitamos el valor a milivoltios entre 3400 y 4100 (Se puede extender aun más ambos límimtes)
  int mv = constrain(raw, 3350, 4250);

  // Calculamos el porcentaje de batería
  uint8_t batt = (mv - 3350) * 100.0 / (4250 - 3350);

  //Serial.print(raw);
  Serial.print("Batería: ");
  Serial.print(batt);
  Serial.println("%");
  
  /*  
  Serial.print("bateria: ");  //para depurar valor leído directamente
  Serial.print(raw);
  */

  return batt;

}

//Función encoder
void encoderA() {
  if (digitalRead(PIN_EN_CLK) == digitalRead(PIN_EN_DT))
    counter--;
  else
    counter++;
}

void encoderB() {
  if (digitalRead(PIN_EN_CLK) != digitalRead(PIN_EN_DT))
    counter--;
  else
    counter++;
}

void doSensor(uint8_t txBuffer[]) {
  // Actualizamos los sensores
  getBME();           // Lee temperatura y presión del BMP280
  getDallasTemp();    // Lee temperatura del suelo

  // Inicializamos el buffer a cero
  memset(txBuffer, 0, TX_BUFFER_SIZE);

  uint16_t temperatura = temp * 100;
  uint32_t presion = press * 100;
  uint16_t tempSuelo = dallasTemp * 100;
  uint16_t humSuelo = getHum();  // Sensor de humedad de suelo
  uint16_t lluvia = getRain();   // Sensor de lluvia
  uint16_t bateria = getBatt();

  // Leer el contador de forma segura
  noInterrupts();  // Desactiva interrupciones temporalmente
  int safeCounter = counter;
  interrupts();    // Vuelve a activar interrupciones

  // Asignación de datos al buffer (ajustado sin humedad del aire)
  txBuffer[0] = byte(temperatura);
  txBuffer[1] = temperatura >> 8;

  txBuffer[2] = byte(presion);
  txBuffer[3] = presion >> 8;
  txBuffer[4] = presion >> 16;
  txBuffer[5] = presion >> 24;

  txBuffer[6] = byte(tempSuelo);
  txBuffer[7] = tempSuelo >> 8;

  txBuffer[8] = byte(humSuelo);
  txBuffer[9] = humSuelo >> 8;

  txBuffer[10] = byte(lluvia);
  txBuffer[11] = lluvia >> 8;

  txBuffer[12] = safeCounter;
  txBuffer[13] = safeCounter >> 8;

  txBuffer[14] = byte(bateria);
  txBuffer[15] = bateria >> 8;
}
