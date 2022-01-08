#include "link.h"
#include "buildopt.h"

#define RADIO_DOWNLINK_FREQUENCY 435.0
#define RADIO_UPLINK_FREQUENCY 435.0

#define RADIO_RF_POWER 20

#define CALL_SIGN "SBDNIC"

#define RADIO_CONNECTION_UNSUCCESSFUL 1

SX1278 radio = new Module(RFM96_NSS_PIN, RFM96_DIO0_PIN, RFM96_RESET_PIN, RFM96_DIO1_PIN);
AFSKClient audio(&radio, RFM96_DIO2_PIN);
AX25Client ax25(&audio);

#define CHECK(x) do { \
  int retval = (x); \
  if (retval != 0) { \
    return retval; \
  } \
} while (0)

int SerialLink::enable() {
  SBUDNIC_DEBUG_PRINTLN("enabling serial link");
  Serial.begin(9600);
  SBUDNIC_DEBUG_PRINTLN("serial begin called");
  SBUDNIC_DEBUG_PRINTLN("done enabling serial link");
  return 0;
}

int SerialLink::disable() {
  Serial.end();
  return 0;
}

int SerialLink::transmit(char* data, uint16_t size) {
  SBUDNIC_DEBUG_PRINTLN("beginning serial transmit");
  Serial.println(data);
  SBUDNIC_DEBUG_PRINTLN("serial println called");
  SBUDNIC_DEBUG_PRINTLN("done with serial transmit");
  return 0;
}

char SerialLink::receive(long timeout) {
  SBUDNIC_DEBUG_PRINTLN("beginning serial receive");
  long start = millis();
  while (millis() - start < timeout && !Serial.available()) {}
  int received = Serial.read();
  if (received == -1) {
    return 0;
  }
  return received;
}

RFM96Link::RFM96Link() {

  pinMode(RFM96_PWR_PIN, OUTPUT);

  digitalWrite(RFM96_PWR_PIN, HIGH);
}

int RFM96Link::enable() {
  SBUDNIC_DEBUG_PRINTLN("enabling RFM96");
  digitalWrite(RFM96_PWR_PIN, LOW);
  SBUDNIC_DEBUG_PRINTLN("RFM96 power turned on");

  SBUDNIC_DEBUG_PRINTLN("RFM96 object created");
  
  CHECK(radio.beginFSK(RADIO_DOWNLINK_FREQUENCY, 1.2, 125.0, 10, 16, false));
  SBUDNIC_DEBUG_PRINTLN("RFM96 beginFSK called");
  CHECK(radio.setOutputPower(RADIO_RF_POWER));
  SBUDNIC_DEBUG_PRINTLN("RFM96 setOutputPower called");
  CHECK(radio.setCurrentLimit(140));
  SBUDNIC_DEBUG_PRINTLN("RFM96 setCurrentLimit called");
  CHECK(ax25.begin(CALL_SIGN));
  SBUDNIC_DEBUG_PRINTLN("RFM96 ax25 begin called");
  SBUDNIC_DEBUG_PRINTLN("done enabling RFM96");
  return 0;
}

int RFM96Link::disable() {
  SBUDNIC_DEBUG_PRINTLN("disabling RFM96");
  digitalWrite(RFM96_PWR_PIN, HIGH);
  SBUDNIC_DEBUG_PRINTLN("RFM96 power turned off");
  SBUDNIC_DEBUG_PRINTLN("done disabling RFM96");
  return 0;
}

int RFM96Link::transmit(char* data, uint16_t size) {
  SBUDNIC_DEBUG_PRINTLN("beginning RFM96 transmit");
  CHECK(ax25.transmit(data, CALL_SIGN));
  SBUDNIC_DEBUG_PRINTLN("RFM96 ax25 transmit called");
  SBUDNIC_DEBUG_PRINTLN("done with RFM96 transmit");
}

char RFM96Link::receive(long timeout) {
  return 0;
}
