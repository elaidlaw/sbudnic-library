#ifndef SBUDNIC_LINK_H
#define SBUDNIC_LINK_H

#include <RadioLib.h>

#include "pins.h"
#include "ssdv.h"

//Class that is responsible for uplink and downlink between satellite and GS, including via Serial, RFM96, and RFM96 with LoRA. 
class LinkInterface {
  public:
    virtual ~LinkInterface(){};
    virtual int enable() = 0;
    virtual int disable() = 0;
    // transmit data using AX.25 over AFSK
    virtual int transmit(char* data) = 0;
    virtual int transmit(byte* data, uint16_t count) = 0;
    // receive for timeout milliseconds and return the received command
    // if there is one or 0x00 otherwise
    virtual char receive(byte* command) = 0;
};

class RFM96Link: public LinkInterface {

  public:
    RFM96Link();
    ~RFM96Link(){}
    int enable();
    int disable();
    int transmit(char* data);
    int transmit(byte* data, uint16_t count) {};
    char receive(byte* command);
};

class RFM96LoRALink: public LinkInterface {

  public:
    RFM96LoRALink();
    ~RFM96LoRALink(){}
    int enable();
    int disable();
    int transmit(char* data);
    int transmit(byte* data, uint16_t count);
    char receive(byte* command);
};

class SerialLink: public LinkInterface {
  public:
    SerialLink(){}
    ~SerialLink(){}
    int enable();
    int disable();
    int transmit(char* data);
    int transmit(byte* data, uint16_t count);
    char receive(byte* command);
};

#endif
