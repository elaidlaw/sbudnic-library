#ifndef SBUDNIC_CONFIG_H
#define SBUDNIC_CONFIG_H

#include <stdint.h>

#define CONFIG_KEY "config"

#define CAM_RES_320x240 0
#define CAM_RES_640x480 1
#define CAM_RES_1024x768 2
#define CAM_RES_1280x960 3
#define CAM_RES_1600x1200 4
#define CAM_RES_2048x1536 5
#define CAM_RES_2592x1944 6

//Data structure to store our satellite's config
typedef struct {
  uint16_t restartCount;
  uint16_t tacticalRestartCount;
  bool camera1Enabled;
  bool camera2Enabled;
  bool safeToOperate;
  bool loraEnabled;
  long downlinkInterval;
  uint8_t resolution;
  uint16_t uplinkDuration;
  uint16_t uplinkDuringSleepInterval;
  uint16_t telemetryBufferInterval;
  char callSign[7];
  float frequency;
} ConfigData;

//Class that handles writing/retrieval of the config
class Config {
  public:
    static void load();
    static void save();
    static void reset();
    
    inline static ConfigData data = {0,1,true,true,false,true,600,0,30,600,60,"SBDNIC",435.0};
};
#endif