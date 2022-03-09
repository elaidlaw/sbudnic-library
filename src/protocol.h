#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "link.h"

#define HEADER_LENGTH 7
#define PHOTO_BODY_LENGTH 128
#define TELEMETRY_BODY_LENGTH 200

#define COMMAND_HEADER "SBDNIC"
#define UPLINK_TEST_COMMAND "HI"
#define UPLINK_TEST_RESPONSE "Hello, World!"
#define AFSK_ENABLE_COMMAND "A9"
#define AFSK_ENABLE_RESPONSE "AFSK on"
#define LORA_ENABLE_COMMAND "L5"
#define LORA_ENABLE_RESPONSE "LoRa on"
#define CAMERA1_ENABLE_COMMAND "C1"
#define CAMERA1_ENABLE_RESPONSE "Cam1 on"
#define CAMERA1_DISABLE_COMMAND "M8"
#define CAMERA1_DISABLE_RESPONSE "Cam1 off"
#define CAMERA2_ENABLE_COMMAND "JU"
#define CAMERA2_ENABLE_RESPONSE "Cam2 on"
#define CAMERA2_DISABLE_COMMAND "B7"
#define CAMERA2_DISABLE_RESPONSE "Cam2 off"
#define RESET_COMMAND "R6"
#define RESET_RESPONSE "Reset"
#define SLEEP_COMMAND "SG"
#define SLEEP_RESPONSE "Goodnight "
#define DOWNLINK_INTERVAL_COMMAND "TW"
#define DOWNLINK_INTERVAL_RESPONSE "Tempo "
#define RESET_PERSISTENT_COMMAND "MF"
#define RESET_PERSISTENT_RESPONSE "Config reset"

class LinkProtocol {
  public:
    static void getPhotoStart(uint8_t* out, uint8_t photoId, uint16_t numPackets, uint8_t cameraId);
    static void getPhotoEnd(uint8_t* out, uint8_t photoId, uint16_t numPackets, uint8_t cameraId);
    static void getPhotoPacket(uint8_t* out, uint8_t photoId, uint16_t packetNumber, uint8_t* data);
    static void getTelemetryPacket(uint8_t* out, uint16_t numDataPoints);
    static void getTelemetryPacket(uint8_t* out, uint16_t numDataPoints, uint8_t* data);
    static void getRestartPacket(uint8_t* out);

    static int processMessage(char* in, LinkInterface* link);
    static constexpr void(* resetFunction) (void) = 0;
};

#endif