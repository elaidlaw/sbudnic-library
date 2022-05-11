#include "deployment.h"
#include "wdt.h"

Deployment::Deployment() {
  pinMode(DEPLOY_PWR_PIN, OUTPUT);
  digitalWrite(DEPLOY_PWR_PIN, LOW);
}

/**
 * @brief - function to trigger deployment of the satellite
 * 
 * @return int - 0 if successfuly deployed
 */
int Deployment::deploy() {
  digitalWrite(DEPLOY_PWR_PIN, HIGH);
  WDT::wddelay(5000);
  digitalWrite(DEPLOY_PWR_PIN, LOW);
  return 0;
}