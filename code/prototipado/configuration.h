#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define PIN_DS18 (23)
#define PIN_HUM (36)
#define PIN_LLUVIA (39)
#define PIN_BAT (34)
#define BME_SCL (22)
#define BME_SDA (21)
#define PIN_EN_CLK (13)
#define PIN_EN_DT (12)
#define PIN_EN_SW (15)

OneWire oneWire(PIN_DS18);
DallasTemperature dTemp(&oneWire);

extern float temp;
extern float hum;
extern float press;
extern float dallasTemp;
extern int counter;

#define ENABLE_DEBUG 1