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
#define DOWNLINK_INTERVAL_RESPONSE "Down int "
#define RESET_PERSISTENT_COMMAND "MF"
#define RESET_PERSISTENT_RESPONSE "Config reset"
#define SEND_CONFIG_COMMAND "U0"
#define SEND_CONFIG_RESPONSE "Config "
#define TELEMETRY_DUMP_COMMAND "JJ"
#define TELEMETRY_DUMP_RESPONSE "Dump"
#define UPLINK_DURATION_COMMAND "BS"
#define UPLINK_DURATION_RESPONSE "Up duration "
#define UPLINK_DURING_SLEEP_INTERVAL_COMMAND "KN"
#define UPLINK_DURING_SLEEP_INTERVAL_RESPONSE "Up sleep int "
#define TELEMETRY_BUFFER_INTERVAL_COMMAND "5D"
#define TELEMETRY_BUFFER_INTERVAL_RESPONSE "Tel int "
#define CALL_SIGN_COMMAND "ZP"
#define CALL_SIGN_REPONSE "Call "
#define FREQUENCY_COMMAND "EY"
#define FREQUENCY_RESPONSE "Freq "
#define UNKNOWN_RESPONSE "Unknown"

#define SLEEP_CODE 2
#define TELEMETRY_DUMP_CODE 3

class LinkProtocol {
  public:
    static void getPhotoStart(uint8_t* out, uint8_t photoId, uint16_t numPackets, uint8_t cameraId);
    static void getPhotoEnd(uint8_t* out, uint8_t photoId, uint16_t numPackets, uint8_t cameraId);
    static void getPhotoPacket(uint8_t* out, uint8_t photoId, uint16_t packetNumber, uint8_t* data);
    static void getTelemetryPacket(uint8_t* out, uint16_t numDataPoints);
    static void getTelemetryPacket(uint8_t* out, uint16_t numDataPoints, uint8_t* data);
    static void getRestartPacket(uint8_t* out);
    
    static void getUplinkStart(uint8_t* out);
    static void getUplinkEnd(uint8_t* out);

    static void getTelemetryDumpStart(uint8_t* out, uint16_t numDataPoints);
    static void getTelemetryDumpEnd(uint8_t* out, uint16_t numDataPoints);
    static void getTelemetryDumpHeader(uint8_t* out, uint16_t packetNumber);
    static int getTelemetryDumpPacketPart(uint8_t* out, bool sensor1, bool sensor2, long time, float val1, float val2);

    static int processCommand(char* in, LinkInterface* link);
    static void tacticalRestart();
    static constexpr void(* resetFunction) (void) = 0;
};

#endif