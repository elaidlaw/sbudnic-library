#ifndef SBUDNIC_CONFIG_H
#define SBUDNIC_CONFIG_H

#define CONFIG_KEY "config"

#define CAM_RES_320x240 0
#define CAM_RES_640x480 1
#define CAM_RES_1024x768 2
#define CAM_RES_1280x960 3
#define CAM_RES_1600x1200 4
#define CAM_RES_2048x1536 5
#define CAM_RES_2592x1944 6

typedef struct {
  bool camera1Enabled;
  bool camera2Enabled;
  bool safeToOperate;
  uint8_t resolution;
} ConfigData;

class Config {
  public:
    static void load();
    static void save();
    static void reset();
    
    static ConfigData data;
};

#endif