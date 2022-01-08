#ifndef CAMERA_H
#define CAMERA_H

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

class CameraInterface {
  public:
    virtual ~CameraInterface(){};
    virtual int enable() = 0;
    virtual int disable() = 0;
    virtual uint32_t takePicture() = 0;
    virtual int readData(uint8_t* data, size_t size) = 0;
    virtual size_t populate_buffer(uint8_t *b) = 0;
    virtual int transmit_packets(uint32_t length, uint8_t *jpeg_buffer, uint8_t *output_buffer) = 0;
};

class Camera: public CameraInterface {
  private:
    ssdv_t ssdv;
    uint8_t pkt[SSDV_PKT_SIZE], b[128];
    int powerPin;
    size_t jpegLen;
  public:
    Camera(uint16_t id);
    ~Camera(){};
    int enable();
    int disable();
    uint32_t takePicture();
    int readData(uint8_t* data, size_t size);
    size_t populate_buffer(uint8_t *b);
    int transmit_packets(uint32_t length, uint8_t *jpeg_buffer, uint8_t *output_buffer);
};

#endif