#include "sensors.h"
#include <Adafruit_ADS1X15.h>
#include <MaximWire.h>
#include "buildopt.h"

// Adafruit_ADS1015 ads;




//TODO: CHANGE THIS SENSOR ID TO THE ONE WERE ACTUALLY USING!


Sensors::Sensors() {
  pinMode(SENSORS_PWR_PIN, OUTPUT);
  // digitalWrite(SENSORS_PWR_PIN, LOW);
}

int Sensors::enable() {
  SBUDNIC_DEBUG_PRINTLN("enabling sensors");
  pinMode(SENSORS_PWR_PIN, OUTPUT);
  digitalWrite(SENSORS_PWR_PIN, HIGH);
  // SBUDNIC_DEBUG_PRINTLN(ads.begin(0x48));
  SBUDNIC_DEBUG_PRINTLN("enabled sensors");
}

int Sensors::disable() {
  pinMode(SENSORS_PWR_PIN, OUTPUT);
  digitalWrite(SENSORS_PWR_PIN, LOW);
}

float Sensors::readTempSensor(int id) {
  MaximWire::Bus tempSensorBus(TEMP_SENSOR_PIN);
  MaximWire::DS18B20 tempSensor0("283E660B000000F9");
  MaximWire::DS18B20 tempSensor1("285C930C000000A6");
  MaximWire::DS18B20 tempSensor2("28FBB60D000000DA");
  float temp = 0.0;
  
  switch(id) {
  case 0:
    tempSensor0.Update(tempSensorBus);
    temp = tempSensor0.GetTemperature<float>(tempSensorBus);
    break;
  case 1:
    tempSensor1.Update(tempSensorBus);
    temp = tempSensor1.GetTemperature<float>(tempSensorBus);
    break;
  case 2:
    tempSensor2.Update(tempSensorBus);
    temp = tempSensor2.GetTemperature<float>(tempSensorBus);
    break;
  }
  return temp;
}

float Sensors::readLight(int id) {
  int out = -1;
  // SBUDNIC_DEBUG_PRINTLN("sensors switch statement");
  switch(id) {
    case 0: {
      out = analogRead(LIGHT_SENSOR_0_PIN);
      break;
      }
    case 1: {
      out = analogRead(LIGHT_SENSOR_1_PIN);
      break;
      }
    case 2: {
      out = analogRead(LIGHT_SENSOR_2_PIN);
      break;
      }
    case 3: {
      out = analogRead(LIGHT_SENSOR_3_PIN);
      break;
      }
    case 4: {
      out = analogRead(LIGHT_SENSOR_4_PIN);
      break;
      }
    case 5: {
      out = analogRead(LIGHT_SENSOR_5_PIN);
      break;
      }
  }

  return out / 1024.0;
}

void Sensors::readAllSensors(char* out) {
  //change to iterate over addr rather than just numebr..
  sprintf(out, "T:");
  for(uint16_t i = 0; i < 3; i++) {
    float temp = readTempSensor(i);
    SBUDNIC_DEBUG_PRINTLN("temp");
    SBUDNIC_DEBUG_PRINTLN(temp);
    sprintf(out, "%s%.2f|", out, temp);
  }
 
  sprintf(out, "%sL:", out);
  for(uint16_t i = 0; i < 6; i++) {
    float light = readLight(i);
    sprintf(out, "%s%.3f|", out, light);
  }
}