#include "deployment.h"

Deployment::Deployment() {

}

int Deployment::deployAntenna() {
  digitalWrite(ANT_DEPLOY_PWR_PIN, LOW);
  delay(5000);
  digitalWrite(ANT_DEPLOY_PWR_PIN, HIGH);
  return 0;
}

int Deployment::deployAOCS() {
  digitalWrite(AOCS_DEPLOY_PWR_PIN, LOW);
  delay(5000);
  digitalWrite(AOCS_DEPLOY_PWR_PIN, HIGH);
  return 0;
}
