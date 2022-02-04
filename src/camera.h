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
    virtual int readData(uint8_t* data, size_t size, int photoId) = 0;
    virtual int transmit_packets(uint32_t length, uint8_t *jpeg_buffer, uint8_t *output_buffer) = 0;
};

class Camera: public CameraInterface {
  private:
    ssdv_t ssdv;
    uint8_t pkt[SSDV_PKT_SIZE], b[128];
    int powerPin;
    int csPin;
    size_t jpegLen;
    ArduCAM camera;
  public:
    Camera(uint16_t id);
    ~Camera(){};
    int enable();
    int disable();
    uint32_t takePicture();
    int readData(uint8_t* data, size_t size, int photoId);
    int transmit_packets(uint32_t length, uint8_t *jpeg_buffer, uint8_t *output_buffer);
};

#endif