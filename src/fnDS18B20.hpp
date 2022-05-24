#include <Arduino.h>
#include "macros_sensores.hpp"
int DS18B20_Init()
{
  pinMode(pinDS18B20, OUTPUT);
  digitalWrite(pinDS18B20, HIGH);
  delayMicroseconds(5);
  digitalWrite(pinDS18B20, LOW);
  delayMicroseconds(750);//480-960
  digitalWrite(pinDS18B20, HIGH);
  pinMode(pinDS18B20, INPUT);
  int t = 0;
  while (digitalRead(pinDS18B20))
  {
    t++;
    if (t > 60) return false;
    delayMicroseconds(1);
  }
  t = 480 - t;
  pinMode(pinDS18B20, OUTPUT);
  delayMicroseconds(t);
  digitalWrite(pinDS18B20, HIGH);
  return true;
}

void DS18B20_Write(byte data)
{
  pinMode(pinDS18B20, OUTPUT);
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(pinDS18B20, LOW);
    delayMicroseconds(10);
    if (data & 1) digitalWrite(pinDS18B20, HIGH);
    else digitalWrite(pinDS18B20, LOW);
    data >>= 1;
    delayMicroseconds(50);
    digitalWrite(pinDS18B20, HIGH);
  }
}

byte DS18B20_Read()
{
  pinMode(pinDS18B20, OUTPUT);
  digitalWrite(pinDS18B20, HIGH);
  delayMicroseconds(2);
  byte data = 0;
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(pinDS18B20, LOW);
    delayMicroseconds(1);
    digitalWrite(pinDS18B20, HIGH);
    pinMode(pinDS18B20, INPUT);
    delayMicroseconds(5);
    data >>= 1;
    if (digitalRead(pinDS18B20)) data |= 0x80;
    delayMicroseconds(55);
    pinMode(pinDS18B20, OUTPUT);
    digitalWrite(pinDS18B20, HIGH);
  }
  return data;
}
 
int TempRead()
{
  if (!DS18B20_Init()) return 0;
  DS18B20_Write (0xCC); // Send skip ROM command
  DS18B20_Write (0x44); // Send reading start conversion command
  if (!DS18B20_Init()) return 0;
  DS18B20_Write (0xCC); // Send skip ROM command
  DS18B20_Write (0xBE); // Read the register, a total of nine bytes, the first two bytes are the conversion value
  int temp = DS18B20_Read (); // Low byte
  temp |= DS18B20_Read () << 8; // High byte
  return temp;
} 