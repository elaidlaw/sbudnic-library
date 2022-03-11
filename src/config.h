#ifndef SBUDNIC_CONFIG_H
#define SBUDNIC_CONFIG_H

#define CONFIG_KEY "config"

typedef struct {
  bool camera1Enabled;
  bool camera2Enabled;
  bool safeToOperate;
  bool loraEnabled;
  long downlinkInterval;
} ConfigData;

class Config {
  public:
    static void load();
    static void save();
    static void reset();
    
    inline static ConfigData data;
};

#endif