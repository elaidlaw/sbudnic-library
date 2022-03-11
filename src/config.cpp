#include "config.h"


#include "KVStore.h"
#include "kvstore_global_api.h"

void Config::load() {
  kv_get(CONFIG_KEY, (uint8_t*)&Config::data, sizeof(Config::data), 0);
}

void Config::save() {
  kv_set(CONFIG_KEY, (uint8_t*)&Config::data, sizeof(Config::data), 0);
}

void Config::reset() {
  kv_reset("/kv/");
  Config::data.camera1Enabled = true;
  Config::data.camera2Enabled = true;
  Config::data.safeToOperate = false;
  Config::data.loraEnabled = true;
  Config::data.downlinkInterval = 600;
  kv_set(CONFIG_KEY, (uint8_t*)&Config::data, sizeof(Config::data), 0);
}