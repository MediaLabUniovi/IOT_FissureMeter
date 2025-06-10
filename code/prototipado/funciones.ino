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

int getHum(){

  int humS = analogRead(PIN_HUM);
  Serial.print("Humedad suelo: ");
  Serial.println(humS);

  return humS;
}

int getRain(){

  int rain = analogRead(PIN_LLUVIA);
  Serial.print("Lluvia: ");
  Serial.println(rain);

  return rain;
}

int getBat(){
  
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

