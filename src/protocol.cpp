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

void LinkProtocol::tacticalRestart() {
    Config::data.tacticalRestartCount += 1;
    Config::save();
    resetFunction();
}

void LinkProtocol::getTelemetryDumpStart(uint8_t* out, uint16_t numDataPoints) {
    sprintf((char*)out, "DS%05d", numDataPoints);
}

void LinkProtocol::getTelemetryDumpEnd(uint8_t* out, uint16_t numDataPoints) {
    sprintf((char*)out, "DE%05d", numDataPoints);
}

void LinkProtocol::getTelemetryDumpHeader(uint8_t* out, uint16_t packetNumber) {
    sprintf((char*)out, "DD%05d", packetNumber);
}

int LinkProtocol::getTelemetryDumpPacketPart(uint8_t* out, bool sensor1, bool sensor2, long time, float val1, float val2) {
    if (sensor1 && sensor2) {
        sprintf((char*)out, "%06d|%+02.02f|%+02.02f|", time, val1, val2);
        return 21;
    }
    if (sensor1) {
        sprintf((char*)out, "%06d|%+02.02f|", time, val1);
        return 14;
    }
    if (sensor2) {
        sprintf((char*)out, "%06d|%+02.02f|", time, val2);
        return 14;
    }
}

int LinkProtocol::processCommand(char* in, LinkInterface* link) {
    // assuming that the link is enabled, and leaving it enabled
    SBUDNIC_DEBUG_PRINTLN("process");
    SBUDNIC_DEBUG_PRINTLN(in);
    SBUDNIC_DEBUG_PRINTLN(strncmp(in, COMMAND_HEADER, 6));
    if (strncmp(in, COMMAND_HEADER, 6) != 0) {
        return -1;
    }
    int code = 0;
    in = in + 6;
    SBUDNIC_DEBUG_PRINTLN("header good");
    char out[256];
    if (strncmp(in, UPLINK_TEST_COMMAND, 2) == 0) {
        sprintf(out, "UR %s  %s", UPLINK_TEST_COMMAND, UPLINK_TEST_RESPONSE);
    } else if (strncmp(in, RESET_COMMAND, 2) == 0) {
        sprintf(out, "UR %s  %s", RESET_COMMAND, RESET_RESPONSE);
        // transmit now because Arduino is getting reset
        link->transmit(out);
        LinkProtocol::tacticalRestart();
    } else if (strncmp(in, RESET_PERSISTENT_COMMAND, 2) == 0) {
        sprintf(out, "UR %s  %s", RESET_PERSISTENT_COMMAND, RESET_PERSISTENT_RESPONSE);
        Config::reset();
    } else if (strncmp(in, SEND_CONFIG_COMMAND, 2) == 0) {
        sprintf(out, "UR %s  %s%06d %06d %d %d %d %d %06d %06d %06d %06d %s",
            SEND_CONFIG_COMMAND,
            SEND_CONFIG_RESPONSE,
            Config::data.restartCount,
            Config::data.tacticalRestartCount,
            Config::data.safeToOperate,
            Config::data.camera1Enabled,
            Config::data.camera2Enabled,
            Config::data.loraEnabled,
            Config::data.downlinkInterval,
            Config::data.uplinkDuration,
            Config::data.uplinkDuringSleepInterval,
            Config::data.telemetryBufferInterval,
            Config::data.callSign
            );
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
        code = SLEEP_CODE;
    } else if (strncmp(in, DOWNLINK_INTERVAL_COMMAND, 2) == 0) {
        in = in + 2;
        in[6] = '\0';
        long seconds = atoi(in);
        SBUDNIC_DEBUG_PRINTLN(seconds);
        sprintf(out, "UR %s  %s%06d", DOWNLINK_INTERVAL_COMMAND, DOWNLINK_INTERVAL_COMMAND, seconds);
        Config::data.downlinkInterval = seconds;
        Config::save();
    } else if (strncmp(in, UPLINK_DURATION_COMMAND, 2) == 0) {
        in = in + 2;
        in[6] = '\0';
        long seconds = atoi(in);
        SBUDNIC_DEBUG_PRINTLN(seconds);
        sprintf(out, "UR %s  %s%06d", UPLINK_DURATION_COMMAND, UPLINK_DURATION_RESPONSE, seconds);
        Config::data.uplinkDuration = seconds;
        Config::save();
    } else if (strncmp(in, UPLINK_DURING_SLEEP_INTERVAL_COMMAND, 2) == 0) {
        in = in + 2;
        in[6] = '\0';
        long seconds = atoi(in);
        SBUDNIC_DEBUG_PRINTLN(seconds);
        sprintf(out, "UR %s  %s%06d", UPLINK_DURING_SLEEP_INTERVAL_COMMAND, UPLINK_DURING_SLEEP_INTERVAL_RESPONSE, seconds);
        Config::data.uplinkDuringSleepInterval = seconds;
        Config::save();
    } else if (strncmp(in, TELEMETRY_BUFFER_INTERVAL_COMMAND, 2) == 0) {
        in = in + 2;
        in[6] = '\0';
        long seconds = atoi(in);
        SBUDNIC_DEBUG_PRINTLN(seconds);
        sprintf(out, "UR %s  %s%06d", TELEMETRY_BUFFER_INTERVAL_COMMAND, TELEMETRY_BUFFER_INTERVAL_RESPONSE, seconds);
        Config::data.telemetryBufferInterval = seconds;
        Config::save();
    } else if (strncmp(in, CALL_SIGN_COMMAND, 2) == 0) {
        in = in + 2;
        char lengthStr[2];
        lengthStr[0] = in[0];
        lengthStr[1] = '\0';
        int length = 0;
        in = in + 1;
        in[length] = '\0';
        strcpy(Config::data.callSign, in);
        sprintf(out, "UR %s  %s%06d", CALL_SIGN_COMMAND, CALL_SIGN_REPONSE, Config::data.callSign);
        Config::save();
    } else if (strncmp(in, TELEMETRY_DUMP_COMMAND, 2) == 0) {
        sprintf(out, "UR %s  %s", TELEMETRY_DUMP_COMMAND, TELEMETRY_DUMP_RESPONSE);
        code = TELEMETRY_DUMP_CODE;
    } else if (strncmp(in, FREQUENCY_COMMAND, 2) == 0) {
        // 400-500
        in = in + 2;
        in[7] = '\0';
        float freq1 = atof(in);
        in[15] = '\0';
        float freq2 = atof(in + 8);
        sprintf(out, "UR %s  %s%03.03d %03.03d", FREQUENCY_COMMAND, FREQUENCY_RESPONSE, freq1, freq2);
        if (freq1 == freq2 && freq1 > 400 && freq1 < 500) {
            Config::data.frequency = freq1;
            Config::save();
        }
    } else {
        in[2] = '\0';
        sprintf(out, "UR %s  %s", in, UNKNOWN_RESPONSE);
    }
    link->transmit(out);
    return code;
}