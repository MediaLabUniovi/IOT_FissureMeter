#include <Arduino.h>
#include "configuracion.hpp"
#include "macros_sensores.hpp"
#include "fnDS18B20.hpp"
#include <Seeed_BME280.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "STM32LowPower.h"

// Sensor BME280
BME280 sensorBME280;

// Variables
uint8_t id_msg=0;
volatile int counter=0;
float DS18B20_tempC=0;
int i_hum=0;
int i_rain=0;
float BME280_pres=0;
float BME280_tempC=0;
float BME280_hum=0;
int i_Vbat=0;

// ******** Encoder
void read_encoder() {
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
// ******** Encoder

// Sends
int send_encTicks=0;
int send_DS18B20_tempC=0;
int send_i_hum=0;
int send_i_rain=0;
int send_BME280_pres=0;
int send_BME280_tempC=0;
int send_BME280_hum=0;
int send_i_Vbat=0;

const lmic_pinmap lmic_pins = {
    .nss = PB12,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = PA3,
    .dio = {PA0, PA1, LMIC_UNUSED_PIN},
};

void os_getArtEui(u1_t *buf) {}
void os_getDevEui(u1_t *buf) {}
void os_getDevKey(u1_t *buf) {}

static osjob_t sendjob;


void do_send(osjob_t *j)
{
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND)
  {
    Serial.println(F("OP_TXRXPEND, not sending"));
  }
  else
  {
    u_int8_t mydata[17];

    // Lectura sensores
    id_msg++;
    static int lastCounter = 0;
    if(counter != lastCounter)
      lastCounter = counter;
    DS18B20_tempC=20;
    i_hum=analogRead(pinHUM);
    i_rain=analogRead(pinRAIN);
    i_Vbat=analogRead(pinBAT);
    DS18B20_tempC = TempRead();
    DS18B20_tempC *= 0.0625; // conversion accuracy is 0.0625 / LSB
    BME280_tempC = sensorBME280.getTemperature();
    BME280_hum = sensorBME280.getHumidity();
    BME280_pres = sensorBME280.getPressure();

    // Sends
    send_encTicks=counter;
    send_DS18B20_tempC=DS18B20_tempC*100;
    send_i_hum=i_hum*100;
    send_i_rain=i_rain*100;
    send_BME280_pres=BME280_pres*10;
    send_BME280_tempC=BME280_tempC*100;
    send_BME280_hum=BME280_hum*100;
    send_i_Vbat=i_Vbat;

    // Array
    mydata[0]=id_msg;
    mydata[1]=highByte(send_encTicks);
    mydata[2]=lowByte(send_encTicks);
    mydata[3]=highByte(send_DS18B20_tempC);
    mydata[4]=lowByte(send_DS18B20_tempC);
    mydata[5]=highByte(send_i_hum);
    mydata[6]=lowByte(send_i_hum);
    mydata[7]=highByte(send_i_rain);
    mydata[8]=lowByte(send_i_rain);
    mydata[9]=highByte(send_BME280_pres);
    mydata[10]=lowByte(send_BME280_pres);
    mydata[11]=highByte(send_BME280_tempC);
    mydata[12]=lowByte(send_BME280_tempC);
    mydata[13]=highByte(send_BME280_hum);
    mydata[14]=lowByte(send_BME280_hum);
    mydata[15]=highByte(send_i_Vbat);
    mydata[16]=lowByte(send_i_Vbat);

    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
    Serial.println(F("Packet queued"));
  }
  // Next TX is scheduled after TX_COMPLETE event.
}

void onEvent(ev_t ev)
{
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev)
  {
  case EV_SCAN_TIMEOUT:
    Serial.println(F("EV_SCAN_TIMEOUT"));
    break;
  case EV_BEACON_FOUND:
    Serial.println(F("EV_BEACON_FOUND"));
    break;
  case EV_BEACON_MISSED:
    Serial.println(F("EV_BEACON_MISSED"));
    break;
  case EV_BEACON_TRACKED:
    Serial.println(F("EV_BEACON_TRACKED"));
    break;
  case EV_JOINING:
    Serial.println(F("EV_JOINING"));
    break;
  case EV_JOINED:
    Serial.println(F("EV_JOINED"));
    break;
  case EV_JOIN_FAILED:
    Serial.println(F("EV_JOIN_FAILED"));
    break;
  case EV_REJOIN_FAILED:
    Serial.println(F("EV_REJOIN_FAILED"));
    break;
  case EV_TXCOMPLETE:
    Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
    if (LMIC.txrxFlags & TXRX_ACK)
      Serial.println(F("Received ack"));
    if (LMIC.dataLen)
    {
      Serial.println(F("Received "));
      Serial.println(LMIC.dataLen);
      Serial.println(F(" bytes of payload"));
    }
    // Schedule next transmission
    //os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
    do_send(&sendjob);
    LowPower.deepSleep();   
    break;
  case EV_LOST_TSYNC:
    Serial.println(F("EV_LOST_TSYNC"));
    break;
  case EV_RESET:
    Serial.println(F("EV_RESET"));
    break;
  case EV_RXCOMPLETE:
    // data received in ping slot
    Serial.println(F("EV_RXCOMPLETE"));
    break;
  case EV_LINK_DEAD:
    Serial.println(F("EV_LINK_DEAD"));
    break;
  case EV_LINK_ALIVE:
    Serial.println(F("EV_LINK_ALIVE"));
    break;
  case EV_TXSTART:
    Serial.println(F("EV_TXSTART"));
    break;
  case EV_TXCANCELED:
    Serial.println(F("EV_TXCANCELED"));
    break;
  case EV_RXSTART:
    /* do not print anything -- it wrecks timing */
    break;
  case EV_JOIN_TXCOMPLETE:
    Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
    break;
  default:
    Serial.print(F("Unknown event: "));
    Serial.println((unsigned)ev);
    break;
  }
}

void setup()
{
  LowPower.begin();

  while (!Serial)
    ; // wait for Serial to be initialized
  Serial.begin(115200);
  delay(100); // per sample code on RF_95 test

#ifdef VCC_ENABLE
  // For Pinoccio Scout boards
  pinMode(VCC_ENABLE, OUTPUT);
  digitalWrite(VCC_ENABLE, HIGH);
  delay(1000);
#endif

  // *********  Sensores Set up
  pinMode(pinCLK, INPUT_PULLUP);
  pinMode(pinDATA, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(pinCLK),read_encoder,CHANGE);
  // attachInterrupt(digitalPinToInterrupt(pinDATA),read_encoder,CHANGE);
  LowPower.attachInterruptWakeup(digitalPinToInterrupt(pinCLK), read_encoder, CHANGE, DEEP_SLEEP_MODE);
  LowPower.attachInterruptWakeup(digitalPinToInterrupt(pinDATA), read_encoder, CHANGE, DEEP_SLEEP_MODE);

  pinMode(pinHUM, INPUT);
  pinMode(pinRAIN, INPUT);
  pinMode(pinDS18B20, INPUT);
  pinMode(pinBAT, INPUT);

  Wire.begin();
  Wire.setSCL(pinBME280_SCL);
  Wire.setSDA(pinBME280_SDA);
  if(!sensorBME280.init())
    Serial.println("BME280 error");

  // ******** RFM95W Set up
  // Use SPI1 on the blackpill_f411ce board.
  SPI.setMOSI(PA7);       
  SPI.setMISO(PA6);
  SPI.setSCLK(PA5);
  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

// Set static session parameters. Instead of dynamically establishing a session
// by joining the network, precomputed session parameters are be provided.
#ifdef PROGMEM
  // On AVR, these values are stored in flash and only copied to RAM
  // once. Copy them to a temporary buffer here, LMIC_setSession will
  // copy them into a buffer of its own again.
  uint8_t appskey[sizeof(APPSKEY)];
  uint8_t nwkskey[sizeof(NWKSKEY)];
  memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
  memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
  LMIC_setSession(0x13, DEVADDR, nwkskey, appskey);
#else
  // If not running an AVR with PROGMEM, just use the arrays directly
  LMIC_setSession(0x13, DEVADDR, NWKSKEY, APPSKEY);
#endif

#if defined(CFG_eu868)

  LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
  LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI); // g-band
  LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
  LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
  LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
  LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
  LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
  LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
  LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK, DR_FSK), BAND_MILLI);   // g2-band

#endif

  // Disable link check validation
  LMIC_setLinkCheckMode(0);

  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF9;

  // Set data rate and transmit power for uplink
  LMIC_setDrTxpow(DR_SF7, 14);

  // Start job
  do_send(&sendjob);
  LowPower.deepSleep();  
}

void loop()
{
  os_runloop_once();
}