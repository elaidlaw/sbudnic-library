#include "deployment.h"
#include "wdt.h"

Deployment::Deployment() {
  pinMode(DEPLOY_PWR_PIN, OUTPUT);
  digitalWrite(DEPLOY_PWR_PIN, LOW);
}

int Deployment::deploy() {
  digitalWrite(DEPLOY_PWR_PIN, HIGH);
  WDT::wddelay(5000);
  digitalWrite(DEPLOY_PWR_PIN, LOW);
  return 0;
}