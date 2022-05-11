#ifndef SBUDNIC_WDT_H
#define SBUDNIC_WDT_H

#define SBUDNIC_WDT_TIMEOUT 30

//Interface for Nano 33BLE watchdog timer.

class WDT {
  public:
    static void enable();
    static void reload();
    static void wddelay(long ms);
};

#endif