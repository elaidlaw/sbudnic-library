#include "sensors.h"
#include <Adafruit_ADS1X15.h>
#include <MaximWire.h>

Adafruit_ADS1015 ads;
MaximWire::Bus tempSensorBus(TEMP_SENSOR_PIN);
MaximWire::DS18B20 tempSensor0("28A684E20800004F");
MaximWire::DS18B20 tempSensor1("28F726E308000069");

Sensors::Sensors() {
  pinMode(SENSORS_PWR_PIN, OUTPUT);
  digitalWrite(SENSORS_PWR_PIN, HIGH);
}

int Sensors::enable() {
  //TODO Maybe change these for light vs temp...
  digitalWrite(SENSORS_PWR_PIN, LOW);
  ads.begin();
}

int Sensors::disable() {
  //TODO Maybe change these for light vs temp...
  digitalWrite(SENSORS_PWR_PIN, HIGH);
}

float Sensors::readTempSensor(int id) {
  float temp = -1;
  switch(id) {
  case 0:
    temp = tempSensor0.GetTemperature<float>(tempSensorBus);
    break;
  case 1:
    temp = tempSensor1.GetTemperature<float>(tempSensorBus);
    break;
  }
  return temp;
}

float Sensors::readLight(int id) {
  int out = -1;
  if(id <= 3) {
    //One of the light sensors is through direct analog 
    out = ads.readADC_SingleEnded(id);
  } else if (id == 4) {
    out = analogRead(LIGHT_SENSOR_4_PIN);
  } else if (id == 5) {
    out = analogRead(LIGHT_SENSOR_5_PIN);
  }
  return out / 1024.0;
}

void Sensors::readAllSensors(char* out) {
  //change to iterate over addr rather than just numebr..
  sprintf(out, "T:");
  for(uint16_t i = 0; i < 2; i++) {
    float temp = readTempSensor(i);
    sprintf(out, "%s%.2f|", out, temp);
  }
  
  sprintf(out, "%sL:", out);
  for(uint16_t i = 0; i < 6; i++) {
    float light = readLight(i);
    sprintf(out, "%s%.3f|", out, light);
  }
}