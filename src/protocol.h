#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define HEADER_LENGTH 7
#define PHOTO_BODY_LENGTH 128
#define TELEMETRY_BODY_LENGTH 200

class LinkProtocol {
  public:
    static void getPhotoStart(uint8_t* out, uint8_t photoId, uint16_t numPackets, uint8_t cameraId);
    static void getPhotoEnd(uint8_t* out, uint8_t photoId, uint16_t numPackets, uint8_t cameraId);
    static void getPhotoPacket(uint8_t* out, uint8_t photoId, uint16_t packetNumber, uint8_t* data);
    static void getTelemetryPacket(uint8_t* out, uint16_t numDataPoints);
    static void getTelemetryPacket(uint8_t* out, uint16_t numDataPoints, uint8_t* data);
    static void getRestartPacket(uint8_t* out);
};

#endif