#ifndef DEPLOYMENT_H
#define DEPLOYMENT_H

#include "Arduino.h"
#include "pins.h"

class DeploymentInterface {
  public:
    virtual ~DeploymentInterface(){};
    virtual int enable() = 0;
    virtual int disable() = 0;
    virtual int deploy() = 0;
};

class Deployment: public DeploymentInterface {
  public:
    Deployment();
    ~Deployment(){}
    int enable(){return 0;}
    int disable(){return 0;}
    int deploy();
};

#endif
