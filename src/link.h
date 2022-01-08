#ifndef SBUDNIC_LINK_H
#define SBUDNIC_LINK_H

#include <RadioLib.h>

#include "pins.h"
#include "ssdv.h"

class LinkInterface {
  public:
    virtual ~LinkInterface(){};
    virtual int enable() = 0;
    virtual int disable() = 0;
    // transmit data using AX.25 over AFSK
    virtual int transmit(char* data, uint16_t count) = 0;
    // receive for timeout milliseconds and return the received command
    // if there is one or 0x00 otherwise
    virtual char receive(long timeout) = 0;
};

class RFM96Link: public LinkInterface {
  // private:
  //   RFM96* radio;
  //   AFSKClient* audio;
  //   AX25Client* ax25;

  public:
    RFM96Link();
    ~RFM96Link(){}
    int enable();
    int disable();
    int transmit(char* data, uint16_t size);
    char receive(long timeout);
};

class SerialLink: public LinkInterface {
  public:
    SerialLink(){}
    ~SerialLink(){}
    int enable();
    int disable();
    int transmit(char* data, uint16_t size);
    char receive(long timeout);
};

#endif
