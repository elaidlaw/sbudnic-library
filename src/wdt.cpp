#include "wdt.h"
#include "Arduino.h"

//Class that supports watchdog timer functionality on Arduino Nano 33BLE

/**
 * @brief Enable WDT with specified timeout
 * 
 */
void WDT::enable() {
  NRF_WDT->CONFIG         = 0x01;
  NRF_WDT->CRV            = SBUDNIC_WDT_TIMEOUT * 32768 + 1;
  NRF_WDT->RREN           = 0x01;
  NRF_WDT->TASKS_START    = 1;
}

/**
 * @brief "Pet" the watchdog to reset the timer
 * 
 */
void WDT::reload() {
  NRF_WDT->RR[0] = WDT_RR_RR_Reload;
}

/**
 * @brief delay function, allows us to delay for longer than watchdog timer   
 * length.
 * 
 * @param ms the number of milliseconds to delay for without triggering WDT.
 */
void WDT::wddelay(long ms) {
  long start = millis();
  while (ms > 29000) {
    WDT::reload();
    delay(29000);
    ms -= millis() - start;
    start = millis();
  }
  WDT::reload();
  delay(ms);
  WDT::reload();
}