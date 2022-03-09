#include "protocol.h"
#include "config.h"

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

int LinkProtocol::processMessage(char* in, LinkInterface* link) {
    if (strcmp(in, COMMAND_HEADER) != 0) {
        return -1;
    }
    in = in + 6;
    char out[64];
    if (strcmp(in, UPLINK_TEST_COMMAND) == 0) {
        sprintf(out, "R  %s  %s", UPLINK_TEST_COMMAND, UPLINK_TEST_RESPONSE);
    } else if (strcmp(in, RESET_COMMAND) == 0) {
        sprintf(out, "R  %s  %s", RESET_COMMAND, RESET_RESPONSE);
        // transmit now because Arduino is getting reset
        link->enable();
        link->transmit(out);
        link->disable();
        LinkProtocol::resetFunction();
    } else if (strcmp(in, RESET_PERSISTENT_COMMAND) == 0) {
        sprintf(out, "R  %s  %s", RESET_PERSISTENT_COMMAND, RESET_PERSISTENT_RESPONSE);
        Config::reset();
    }
    link->enable();
    link->transmit(out);
    link->disable();
    return 0;
}