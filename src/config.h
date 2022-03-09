#ifndef SBUDNIC_CONFIG_H
#define SBUDNIC_CONFIG_H

#define CONFIG_KEY "config"

typedef struct {
  bool camera1Enabled;
  bool camera2Enabled;
  bool safeToOperate;
} ConfigData;

class Config {
  public:
    static void load();
    static void save();
    static void reset();
    
    static ConfigData data;
};

#endif