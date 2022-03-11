#include "protocol.h"
#include "config.h"
#include "buildopt.h"
#include "wdt.h"

void LinkProtocol::getPhotoStart(uint8_t* out, uint8_t photoId, uint16_t numPackets, uint8_t cameraId) {
    sprintf((char*)out, "PS%01d%03d%01d", photoId, numPackets, cameraId);
}

void LinkProtocol::getPhotoEnd(uint8_t* out, uint8_t photoId, uint16_t numPackets, uint8_t cameraId) {
    sprintf((char*)out, "PE%01d%03d%01d", photoId, numPackets, cameraId);
}

void LinkProtocol::getPhotoPacket(uint8_t* out, uint8_t photoId, uint16_t packetNumber, uint8_t* data) {
    sprintf((char*)out, "PD%01d%03d ", photoId, packetNumber);
    memcpy(out + HEADER_LENGTH, data, PHOTO_BODY_LENGTH);
}

void LinkProtocol::getTelemetryPacket(uint8_t* out, uint16_t numDataPoints) {
    sprintf((char*)out, "TD%05d", numDataPoints);
}

void LinkProtocol::getTelemetryPacket(uint8_t* out, uint16_t numDataPoints, uint8_t* data) {
    sprintf((char*)out, "TD%05d", numDataPoints);
    memcpy(out + HEADER_LENGTH, data, PHOTO_BODY_LENGTH);
}

void LinkProtocol::getRestartPacket(uint8_t* out) {
    sprintf((char*)out, "RRRRRRR");
}

void LinkProtocol::getUplinkStart(uint8_t* out) {
    sprintf((char*)out, "US     ");
}

void LinkProtocol::getUplinkEnd(uint8_t* out) {
    sprintf((char*)out, "UE     ");
}

int LinkProtocol::processCommand(char* in, LinkInterface* link) {
    SBUDNIC_DEBUG_PRINTLN("process");
    SBUDNIC_DEBUG_PRINTLN(in);
    SBUDNIC_DEBUG_PRINTLN(strncmp(in, COMMAND_HEADER, 6));
    if (strncmp(in, COMMAND_HEADER, 6) != 0) {
        return -1;
    }
    in = in + 6;
    SBUDNIC_DEBUG_PRINTLN("header good");
    char out[64];
    if (strncmp(in, UPLINK_TEST_COMMAND, 2) == 0) {
        sprintf(out, "UR %s  %s", UPLINK_TEST_COMMAND, UPLINK_TEST_RESPONSE);
    } else if (strncmp(in, RESET_COMMAND, 2) == 0) {
        sprintf(out, "UR %s  %s", RESET_COMMAND, RESET_RESPONSE);
        // transmit now because Arduino is getting reset
        link->enable();
        link->transmit(out);
        link->disable();
        LinkProtocol::resetFunction();
    } else if (strncmp(in, RESET_PERSISTENT_COMMAND, 2) == 0) {
        sprintf(out, "UR %s  %s", RESET_PERSISTENT_COMMAND, RESET_PERSISTENT_RESPONSE);
        Config::reset();
    } else if (strncmp(in, SEND_CONFIG_COMMAND, 2) == 0) {
        sprintf(out, "UR %s  %s%d", SEND_CONFIG_COMMAND, SEND_CONFIG_RESPONSE, Config::data.safeToOperate);
    } else if (strncmp(in, AFSK_ENABLE_COMMAND, 2) == 0) {
        sprintf(out, "UR %s  %s", AFSK_ENABLE_COMMAND, AFSK_ENABLE_RESPONSE);
        Config::data.loraEnabled = false;
        Config::save();
    } else if (strncmp(in, LORA_ENABLE_COMMAND, 2) == 0) {
        sprintf(out, "UR %s  %s", LORA_ENABLE_COMMAND, LORA_ENABLE_RESPONSE);
        Config::data.loraEnabled = true;
        Config::save();
    } else if (strncmp(in, CAMERA1_ENABLE_COMMAND, 2) == 0) {
        sprintf(out, "UR %s  %s", CAMERA1_ENABLE_COMMAND, CAMERA1_ENABLE_RESPONSE);
        Config::data.camera1Enabled = true;
        Config::save();
    } else if (strncmp(in, CAMERA1_DISABLE_COMMAND, 2) == 0) {
        sprintf(out, "UR %s  %s", CAMERA1_DISABLE_COMMAND, CAMERA1_DISABLE_RESPONSE);
        Config::data.camera1Enabled = false;
        Config::save();
    } else if (strncmp(in, CAMERA2_ENABLE_COMMAND, 2) == 0) {
        sprintf(out, "UR %s  %s", CAMERA2_ENABLE_COMMAND, CAMERA2_ENABLE_RESPONSE);
        Config::data.camera2Enabled = true;
        Config::save();
    } else if (strncmp(in, CAMERA2_DISABLE_COMMAND, 2) == 0) {
        sprintf(out, "UR %s  %s", CAMERA2_DISABLE_COMMAND, CAMERA2_DISABLE_RESPONSE);
        Config::data.camera2Enabled = false;
        Config::save();
    } else if (strncmp(in, SLEEP_COMMAND, 2) == 0) {
        in = in + 2;
        in[6] = '\0';
        long seconds = atoi(in);
        sprintf(out, "UR %s  %s %06d", SLEEP_COMMAND, SLEEP_RESPONSE, seconds);
        // transmit now because we're going to sleep
        link->enable();
        link->transmit(out);
        link->disable();
        if (seconds < 86400) {
            WDT::wddelay(seconds * 1000);
        }
        LinkProtocol::resetFunction();
    } else if (strncmp(in, DOWNLINK_INTERVAL_COMMAND, 2) == 0) {
        in = in + 2;
        in[6] = '\0';
        long seconds = atoi(in);
        sprintf(out, "UR %s  %s%06d", DOWNLINK_INTERVAL_COMMAND, DOWNLINK_INTERVAL_COMMAND, seconds);
        Config::data.downlinkInterval = seconds;
        Config::save();
    } else {
        in[2] = '\0';
        sprintf(out, "UR %s  %s", in, UNKNOWN_RESPONSE);
    }
    link->enable();
    link->transmit(out);
    link->disable();
    return 0;
}

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
#define SEND_CONFIG_COMMAND "U0"
#define SEND_CONFIG_RESPONSE "Config "