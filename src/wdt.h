#ifndef SBUDNIC_WDT_H
#define SBUDNIC_WDT_H

#define SBUDNIC_WDT_TIMEOUT 30

class WDT {
  public:
    static void enable();
    static void reload();
    static void wddelay(long ms);
};

#endif