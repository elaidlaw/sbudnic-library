#include "deployment.h"

Deployment::Deployment() {

}

int Deployment::deploy() {
  digitalWrite(DEPLOY_PWR_PIN, LOW);
  delay(5000);
  digitalWrite(DEPLOY_PWR_PIN, HIGH);
  return 0;
}