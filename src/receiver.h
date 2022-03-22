#ifndef RECEIVER_H
#define RECEIVER_H

#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"
#include <Wire.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ssdv.h"
#include "pins.h"

#define REC_WAIT 0
#define REC_DONE 1
#define REC_FULL 2
#define REC_OFF 3

class ReceiverInterface {
  public:
    virtual ~ReceiverInterface(){};
    virtual int enable() = 0;
    virtual int disable() = 0;
    virtual int read_data(uint32_t length, uint8_t *jpeg_buffer, uint8_t *output_buffer) = 0;
    virtual uint8_t get_state() = 0;
    virtual int get_output_pos() = 0;
    void receive_data(int howMany);
    // virtual static void send_settings() = 0;
};


class Receiver: public ReceiverInterface {
  private:
    
  public:
    static ssdv_t ssdv;
    static uint8_t pkt[SSDV_PKT_SIZE];
    static int buf_len;
    static int photoID;
    static bool useCam1;
    static uint8_t state;
    static uint8_t *output_buffer;
    static int output_position;
    Receiver(uint8_t *ob, int blen);
    ~Receiver(){};
    int enable();
    int disable();
    int read_data(uint32_t length, uint8_t *jpeg_buffer, uint8_t *output_buffer);
    uint8_t get_state();
    int get_output_pos();
    static void receive_data(int howMany);
    static void send_settings();
};

#endif