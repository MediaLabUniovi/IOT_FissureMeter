// configuration.h
#pragma once

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <STM32LowPower.h>
#include <STM32RTC.h>

extern Adafruit_BME280 bme280;  // Declaración externa para que otros archivos lo usen

#define pinDS18B20 PB5
#define pinEncoderClk PB9
#define pinEncoderData PB8
#define pinHUM PB1
#define pinBat PA2
#define pinRAIN PB0





