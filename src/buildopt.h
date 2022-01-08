// #define SBUDNIC_DEBUG

#ifdef SBUDNIC_DEBUG
#define SBUDNIC_DEBUG_PRINTLN(x) Serial.print("debug: "); Serial.println(x)
#else
#define SBUDNIC_DEBUG_PRINTLN(x) do { } while(0)
#endif