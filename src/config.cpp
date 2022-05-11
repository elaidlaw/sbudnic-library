#include "config.h"
#include "KVStore.h"
#include "kvstore_global_api.h"

/**
 * @brief - loads the current config data from persistent storage to
 * the config data structure
 */
void Config::load() {
  kv_get(CONFIG_KEY, (uint8_t*)&Config::data, sizeof(Config::data), 0);
}

/**
 * @brief - writes the current config data to persistent storage from
 * the config data structure
 */
void Config::save() {
  kv_set(CONFIG_KEY, (uint8_t*)&Config::data, sizeof(Config::data), 0);
}

/**
 * @brief - resets the config data in persistent storage to its default vals
 */
void Config::reset() {
  kv_reset("/kv/");
  Config::data.restartCount = 0;
  Config::data.tacticalRestartCount = 1;
  Config::data.camera1Enabled = true;
  Config::data.camera2Enabled = true;
  Config::data.safeToOperate = false;
  Config::data.loraEnabled = true;
  Config::data.downlinkInterval = 600;
  Config::data.resolution = 0;
  Config::data.uplinkDuration = 30;
  Config::data.uplinkDuringSleepInterval = 600;
  Config::data.telemetryBufferInterval = 60;
  char* callSign = "SBDNIC";
  Config::data.frequency = 435.0;
  strcpy(Config::data.callSign, callSign);
  kv_set(CONFIG_KEY, (uint8_t*)&Config::data, sizeof(Config::data), 0);
}