#include "configuration.h"

float temp;
float hum;
float press;
int tHum;
int pHum;


const int humedadAire = 550;
const int humedadAgua = 250;


OneWire oneWire(pinDS18B20);
DallasTemperature dTemp(&oneWire);

int counter;
float dallasTemp;

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
    dallasTemp = dTemp.getTempCByIndex(0); // Lee la temperatura del primer sensor encontrado

    if(dallasTemp == DEVICE_DISCONNECTED_C) {
        Serial.println("Error: Sensor desconectado"); // Verifica si el sensor está desconectado
    } else {
        Serial.print("Temperatura suelo: ");
        Serial.print(dallasTemp);
        Serial.println(" °C");
    }
}

void doSensors(){


    //int i_rain=analogRead(pinRAIN);


    getBME();
    
    getDallasTemp();

    // Humedad Suelo
    tHum=analogRead(pinHUM);
    pHum = map(tHum, humedadAire, humedadAgua, 0, 100); //Obtención de valores de humedad y mapeo a porcentaje
    if(pHum > 100) pHum = 100; // Revisar si hay que hacer cota inferior

    Serial.print("Humedad Suelo: ");
    Serial.println(pHum);
    
    Serial.print("Fisurometro coder: ");
    Serial.println(counter);

    batt = analogRead(pinBat);
    Serial.print("Batt: ");
    Serial.println(batt/100);
    
    
}

void do_send() {
if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("Operation pending, not sending"));
    } else {
        // Buffer para almacenar los datos de los floats
        uint8_t message[20];
        
        // Copia las variables float en el buffer de bytes

        // Temperatura suelo
        int temp2 = dallasTemp *100;
        message[0]=byte(temp2);
        message[1]=temp2 >>8;

        // Medida fisura
        int count2 = counter;
        message[2]=byte(count2);
        message[3]=count2 >>8;

        // Temperatura ambiente
        int tempB2 = temp *100;
        message[4]=byte(tempB2);
        message[5]=tempB2 >>8;
        
        // Humedad ambiente
        int humB2 = hum *100;
        message[6]=byte(humB2);
        message[7]=humB2 >>8;

        // Presión ambiente
        int pressB2 = press *100;
        message[8]=byte(pressB2);
        message[9]=pressB2 >>8;
        message[10]=pressB2 >>16;
        message[11]=pressB2 >>32;

        // Humedad suelo
        pHum = pHum *100;
        message[12]=byte(pHum);
        message[13]=pHum >>8;

        // Lluvia
        message[14]=byte(pHum);
        message[15]=pHum >>8;

        // Bateria
        message[16]=byte(batt);
        message[17]=batt >>8;
        
        // Enviar el mensaje
        LMIC_setTxData2(1, message, sizeof(message), 0);
        Serial.println(F("Sending uplink packet with float values..."));
    }
}

void encoderA() {
  if (digitalRead(pinEncoderClk) == digitalRead(pinEncoderData))
    counter--;
  else
    counter++;
}

void encoderB() {
  if (digitalRead(pinEncoderClk) != digitalRead(pinEncoderData))
    counter--;
  else
    counter++;
}
