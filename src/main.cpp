#include <Arduino.h>
#include "SPI.h"
#include <nRF24L01.h>
#include <RF24.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Wire.h>
#include <Adafruit_INA219.h>
#include "RTClib.h"

const int ledRefillPin = 0;
const int ledWaterPin = 1;
const int btnRefillPin = 2;
const int btnWateringPin = 3;
const int btnLimitPin = 4;
const int pumpPin = 5;
const int tftRstPin = 6;
const int tftCsPin = 7;
const int tftDcPin = 8;
const int nrf24CePin = 9;
const int nrf24CsnPin = 10;
const int mosiPin = 11;
const int misoPin = 12;
const int spiClkPin = 13;
const int moistureSensor1Pin = A0;
const int moistureSensor2Pin = A1;
const int moistureSensorPowerPin = A2;
const int potiPin = A3;

bool ledRefillOn = false;
bool ledWaterOn = true;
bool moistureSensorOn = false;
char uuid[50];

Adafruit_ILI9341 tft = Adafruit_ILI9341(tftCsPin, tftDcPin, tftRstPin);

RF24 radio(nrf24CePin, nrf24CsnPin);
const uint8_t receivingAddress[] = "switch";
const uint8_t sendingAddress[] = "wwitc";

Adafruit_INA219 ina219;

RTC_DS3231 ds3231;
DateTime datetime;

void setup()
{
  TCCR0B = TCCR0B & 0b11111000 | 0x02;

  pinMode(ledRefillPin, OUTPUT);
  pinMode(ledWaterPin, OUTPUT);
  pinMode(btnRefillPin, INPUT_PULLUP);
  pinMode(btnWateringPin, INPUT_PULLUP);
  pinMode(btnLimitPin, INPUT_PULLUP);
  pinMode(pumpPin, OUTPUT);
  pinMode(moistureSensor1Pin, INPUT);
  pinMode(moistureSensor2Pin, INPUT);
  pinMode(moistureSensorPowerPin, OUTPUT);
  pinMode(potiPin, INPUT);

  tft.begin();
  tft.setRotation(3);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.fillScreen(ILI9341_BLACK);

  radio.begin();
  radio.setRetries(15, 15);
  radio.setAutoAck(true);
  radio.openWritingPipe(sendingAddress);
  radio.openReadingPipe(1, receivingAddress);
  radio.startListening();

  ina219.begin();

  ds3231.begin();
}

void loop()
{
  tft.fillRect(150, 10, 40, 140, ILI9341_BLACK);
  tft.fillRect(150, 140, 120, 10, ILI9341_BLACK);

  ledRefillOn = !ledRefillOn;
  digitalWrite(ledRefillPin, ledRefillOn);
  tft.setCursor(5, 10);
  tft.print("ledRefillOn=");
  tft.setCursor(150, 10);
  tft.print(ledRefillOn);

  ledWaterOn = !ledWaterOn;
  digitalWrite(ledWaterPin, ledWaterOn);
  tft.setCursor(5, 20);
  tft.print("ledWaterOn=");
  tft.setCursor(150, 20);
  tft.print(ledWaterOn);

  tft.setCursor(5, 30);
  tft.print("btnRefill=");
  tft.setCursor(150, 30);
  tft.print(digitalRead(btnRefillPin));

  tft.setCursor(5, 40);
  tft.print("btnWatering=");
  tft.setCursor(150, 40);
  tft.print(digitalRead(btnWateringPin));

  tft.setCursor(5, 50);
  tft.print("btnLimit=");
  tft.setCursor(150, 50);
  tft.print(digitalRead(btnLimitPin));

  tft.setCursor(5, 60);
  tft.print("radio=");
  tft.setCursor(150, 60);
  if (radio.available())
  {
    radio.read(&uuid, 16 * 2 + 1);
    tft.print(uuid);
  }

  tft.setCursor(5, 70);
  tft.print("moistureSensor1=");
  tft.setCursor(150, 70);
  tft.print(analogRead(moistureSensor1Pin));

  tft.setCursor(5, 80);
  tft.print("moistureSensor2=");
  tft.setCursor(150, 80);
  tft.print(analogRead(moistureSensor2Pin));

  tft.setCursor(5, 90);
  tft.print("poti=");
  tft.setCursor(150, 90);
  int potiVal = analogRead(potiPin);
  tft.print(potiVal);

  tft.setCursor(5, 100);
  tft.print("pump=");
  int pumpVal = potiVal * 255.0 / 1024.0;
  tft.setCursor(150, 100);
  tft.print(pumpVal);
  analogWrite(pumpPin, pumpVal);

  moistureSensorOn = !moistureSensorOn;
  digitalWrite(moistureSensorPowerPin, moistureSensorOn);
  tft.setCursor(5, 110);
  tft.print("moistureSensorOn=");
  tft.setCursor(150, 110);
  tft.print(moistureSensorOn);

  tft.setCursor(5, 120);
  tft.print("current [mA]=");
  tft.setCursor(150, 120);
  tft.print(ina219.getCurrent_mA());

  tft.setCursor(5, 130);
  tft.print("voltage [V]=");
  tft.setCursor(150, 130);
  tft.print(ina219.getBusVoltage_V());

  tft.setCursor(5, 140);
  datetime = ds3231.now();
  tft.print("now=");
  tft.setCursor(150, 140);
  tft.print(datetime.year());
  tft.print("-");
  tft.print(datetime.month());
  tft.print("-");
  tft.print(datetime.day());
  tft.print(" ");
  tft.print(datetime.hour());
  tft.print(":");
  tft.print(datetime.minute());
  tft.print(":");
  tft.print(datetime.second());

  delay(200);
}