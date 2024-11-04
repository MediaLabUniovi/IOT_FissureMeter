// sensors.h
#pragma once

#include <Adafruit_BME280.h>
#include <DallasTemperature.h>
#include <OneWire.h>

extern float temp;
extern float hum;
extern float press;
extern int tHum;
extern int pHum;
extern int rainValue;

extern const int humedadAire;
extern const int humedadAgua;

extern OneWire oneWire;
extern DallasTemperature dTemp;

extern int counter;
extern float dallasTemp;

void getBME();
void getDallasTemp();
void doSensors();
void do_send();
void encoderA();
void encoderB();
