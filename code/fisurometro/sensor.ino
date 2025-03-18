#include "configuration.h"

float temp;
float hum;
float press;
float dallasTemp;
int counter;

void getBME(){

   Serial.print("Temperatura = ");
   temp = bme280.readTemperature();
   Serial.print(temp);
   Serial.println(" °C");

   Serial.print("Humedad = ");
   hum = bme280.readHumidity();
   Serial.print(hum);
   Serial.println(" %");

   Serial.print("Presión = ");
   press = bme280.readPressure() /100.0;
   Serial.print(press);
   Serial.println(" hPa");
}

void getDallasTemp(){
    dTemp.begin();
    dTemp.requestTemperatures(); // Solicita las temperaturas a los sensores
    dallasTemp = dTemp.getTempCByIndex(0); // Lee la temperatura del primer sensor encontrado (float)

    if(dallasTemp == DEVICE_DISCONNECTED_C) {
        Serial.println("Error: Sensor desconectado"); // Verifica si el sensor está desconectado
    } else {
        Serial.print("Temperatura suelo: ");
        Serial.print(dallasTemp);
        Serial.println(" °C");
    }
}

uint16_t getHum(){

  int humS = analogRead(PIN_HUM);
  Serial.print("Humedad suelo: ");
  Serial.println(humS);

  return humS;
}

uint16_t getRain(){

  int rain = analogRead(PIN_LLUVIA);
  Serial.print("Lluvia: ");
  Serial.println(rain);

  return rain;
}

uint16_t getBatt(){
  
  int batt = analogRead(PIN_BAT);
  Serial.print("Batería: ");
  Serial.println(batt);

  return batt;
}


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

  // Ponemos a 0s el buffer de envío
  memset(txBuffer, 0, TX_BUFFER_SIZE);

  uint16_t temperatura = temp *100;
  uint16_t humedad = hum *100;
  uint32_t presion = press *100;
  uint16_t tempSuelo = dallasTemp *100;  // Revisar
  uint16_t humSuelo = getHum();
  uint16_t lluvia = getRain();
  uint16_t bateria = (uint16_t)(getBatt() *3.3 *2 *100)/ 4096;
  
  txBuffer[0] = byte(temperatura);
  txBuffer[1] = temperatura >> 8;

  txBuffer[2] = byte(humedad);
  txBuffer[3] = humedad >> 8;

  txBuffer[4] = byte(presion);
  txBuffer[5] = presion >> 8;
  txBuffer[6] = presion >> 16;
  txBuffer[7] = presion >> 24;

  txBuffer[8] = byte(tempSuelo);
  txBuffer[9] = tempSuelo >> 8;

  txBuffer[10] = byte(humSuelo);
  txBuffer[11] = humSuelo >> 8;

  txBuffer[12] = byte(lluvia);
  txBuffer[13] = lluvia >> 8;

  txBuffer[14] = byte(counter);
  txBuffer[15] = counter >> 8;

  txBuffer[16] = byte(bateria);
  txBuffer[17] = bateria >> 8;
}