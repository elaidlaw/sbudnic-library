#ifndef SBUDNIC_SENSORS_H
#define SBUDNIC_SENSORS_H

#define MAXIMWIRE_EXTERNAL_PULLUP

#include "pins.h"

class SensorsInterface {
  public:
    virtual ~SensorsInterface(){};
    virtual int enable() = 0;
    virtual int disable() = 0;
    virtual float readTempSensor(int id) = 0;
    virtual float readLight(int id) = 0;
    virtual void readAllSensors(char* data) = 0;
};

class Sensors: public SensorsInterface {
  public:
    Sensors();
    ~Sensors(){}
    int enable();
    int disable();
    float readTempSensor(int id);
    float readLight(int id);
    void readAllSensors(char* data);
};

#endif