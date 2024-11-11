//sensors.cpp
#include "configuration.h"
#include "sensors.h"

float temp;
float hum;
float press;
int tHum;
int pHum;
int rainValue;

const int humedadAire = 550;
const int humedadAgua = 250;

// Temporizador para el encoder
const unsigned long interval = 100; 

OneWire oneWire(pinDS18B20);
DallasTemperature dTemp(&oneWire);

int counter;
float dallasTemp;
int batt;

//Sensor BME temperatura, humedad y presión 
void getBME(){

  bme280.takeForcedMeasurement();

    //Serial1.print("Temperatura = ");
    temp = bme280.readTemperature();
    //Serial1.print(temp);
    //Serial1.println(" °C");

    //Serial1.print("Humedad = ");
    hum = bme280.readHumidity();
    //Serial1.print(hum);
    //Serial1.println(" %");

    //Serial1.print("Presión = ");
    press = bme280.readPressure() /100.0;
    //Serial1.print(press);
    //Serial1.println(" hPa");
}

//Función temperatura del suelo
void getDallasTemp(){
    dTemp.requestTemperatures(); // Solicita las temperaturas a los sensores
    dallasTemp = dTemp.getTempCByIndex(0); // Lee la temperatura del primer sensor encontrado

    if(dallasTemp == DEVICE_DISCONNECTED_C) {
        //Serial1.println("Error: Sensor desconectado"); // Verifica si el sensor está desconectado
    } else {
        //Serial1.print("Temperatura suelo: ");
        //Serial1.print(dallasTemp);
        //Serial1.println(" °C");
    }
}

//Medida del sensor de lluvia
void rainSensor(){

  int RainSensorValue=analogRead(pinRAIN);

  if (RainSensorValue < 300) {
    // Heavy rain
    //Serial1.println("Heavy rain");
    //Serial1.println(RainSensorValue);
    //rainValue=2;
  } 
  else if (RainSensorValue <450) {
    // Moderate rain
    //Serial1.println("Moderate rain");
    //Serial1.println(RainSensorValue);
    //rainValue=1;
  } 
  else {
    // No rain
    //Serial1.println("No rain");
    //Serial1.println(RainSensorValue);
    //rainValue=0;
  }

} 

//En esta función se ejecutan todas las medidas de los sensores
void doSensors(){

    getBME();
    
    getDallasTemp();

    rainSensor();

    // Humedad Suelo
    tHum=analogRead(pinHUM);
    pHum = map(tHum, humedadAire, humedadAgua, 0, 100); //Obtención de valores de humedad y mapeo a porcentaje
    if(pHum > 100) pHum = 100; // Revisar si hay que hacer cota inferior

    //Serial1.print("Humedad Suelo: ");
    //Serial1.println(pHum);
    
    //Encoder fisurometro
    //Serial1.print("Fisurometro coder: ");
    //Serial1.println(counter);

    //Medidor de bateria
    batt = analogRead(pinBat);
    //Serial1.print("Batt: ");
    //Serial1.println(batt/100);
    
    
}

//Se prepara el paquete para mandar por LoRa
void do_send() {
if (LMIC.opmode & OP_TXRXPEND) {
        //Serial1.println(F("Operation pending, not sending"));
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
        int pressB2 = press * 100;
        message[8] = byte(pressB2);
        message[9] = pressB2 >> 8;
        message[10] = pressB2 >> 16;
        message[11] = pressB2 >> 24;  // Corrige a 24 bits en lugar de 32 

        // Humedad suelo
        pHum = pHum *100;
        message[12]=byte(pHum);
        message[13]=pHum >>8;

        // Lluvia
        int RainSensorValue = RainSensorValue *100;
        message[14]=byte(RainSensorValue);
        message[15]=RainSensorValue >>8;

        // Bateria
        message[16]=byte(batt);
        message[17]=batt >>8;
        
        // Enviar el mensaje
        LMIC_setTxData2(1, message, sizeof(message), 0);
        //Serial1.println(F("Sending uplink packet with float values..."));
    }
}

//Funciones de medida de los encoders
void encoderA() {
  if (digitalRead(pinEncoderClk) == digitalRead(pinEncoderData)){
    counter--;
    }
  else{
    counter++;
    }

  // Se espera un tiempo antes de dormir el micro por si hay mas cambios del encoder(millis()=0 al despertar el micro)
  if (Encoder && millis() >= interval){LowPower.deepSleep();}//Se vuelve adormir el micro después de activarse la interrupción del encoder si no se está enviando algo

}

void encoderB() {
  if (digitalRead(pinEncoderClk) != digitalRead(pinEncoderData)){
    counter--;
    }
  else{
    counter++;
    }

  if (Encoder && millis() >= interval){LowPower.deepSleep();}//Se vuelve adormir el micro después de activarse la interrupción del encoder si no se está enviando algo

}
