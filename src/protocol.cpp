#include "protocol.h"

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