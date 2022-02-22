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
  MaximWire::DS18B20 tempSensor0("28C2730C000000DE");
  MaximWire::DS18B20 tempSensor1("282A3B0C00000081");
  float temp = 0.0;
  
  switch(id) {
  case 0:
    temp = tempSensor0.GetTemperature<float>(tempSensorBus);
    tempSensor0.Update(tempSensorBus);
    SBUDNIC_DEBUG_PRINTLN("temp");
    SBUDNIC_DEBUG_PRINTLN(temp);
    break;
  case 1:
    temp = tempSensor1.GetTemperature<float>(tempSensorBus);
    tempSensor1.Update(tempSensorBus);
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

  // if(id <= 3) {
  //   //One of the light sensors is through direct analog 
  //   SBUDNIC_DEBUG_PRINTLN("ADC read");
  //   // out = ads.readADC_SingleEnded(id);
  //   out = 
  //   SBUDNIC_DEBUG_PRINTLN("done with ADC read");
  // } else if (id == 4) {
  //   out = analogRead(LIGHT_SENSOR_4_PIN);
  // } else if (id == 5) {
  //   out = analogRead(LIGHT_SENSOR_5_PIN);
  // }
  return out / 1024.0;
}

void Sensors::readAllSensors(char* out) {
  //change to iterate over addr rather than just numebr..
  sprintf(out, "T:");
  for(uint16_t i = 0; i < 2; i++) {
    float temp = readTempSensor(i);
    SBUDNIC_DEBUG_PRINTLN("temp");
    SBUDNIC_DEBUG_PRINTLN(temp);
    sprintf(out, "%s%.2f|", out, temp);
  }
  // MaximWire::Bus bus(TEMP_SENSOR_PIN);
  // MaximWire::DS18B20 device;
  // MaximWire::Discovery discovery = bus.Discover();
  // do {
  //     MaximWire::Address address;
  //     if (discovery.FindNextDevice(address)) {
  //       MaximWire::DS18B20 device(address);
  //       float temp = device.GetTemperature<float>(bus);
  //       SBUDNIC_DEBUG_PRINTLN(temp);
  //       device.Update(bus);  
  //       sprintf(out, "%s%.2f|", out, temp);
  //     } else {
  //       float temp = -1;
  //       sprintf(out, "%s%.2f|", out, temp);
  //     }
  // } while (discovery.HaveMore());
  
  sprintf(out, "%sL:", out);
  for(uint16_t i = 0; i < 6; i++) {
    float light = readLight(i);
    sprintf(out, "%s%.3f|", out, light);
  }
}