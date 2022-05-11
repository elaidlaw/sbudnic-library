#include "link.h"
#include "buildopt.h"
#include "config.h"

#define RADIO_DOWNLINK_FREQUENCY 435.0
#define RADIO_UPLINK_FREQUENCY 435.0

#define RADIO_RF_POWER 20

#define RADIO_CONNECTION_UNSUCCESSFUL 1

SX1278 radio = new Module(RFM96_NSS_PIN, RFM96_DIO0_PIN, RFM96_RESET_PIN, RFM96_DIO1_PIN);
// AFSKClient audio(&radio, RFM96_DIO2_PIN);
// AX25Client ax25(&audio);

#define CHECK(x) do { \
  int retval = (x); \
  if (retval != 0) { \
    return retval; \
  } \
} while (0)

// Serial version of radio connection, used for testing.
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

int SerialLink::transmit(char* data) {
  SBUDNIC_DEBUG_PRINTLN("beginning serial transmit");
  Serial.println(data);
  SBUDNIC_DEBUG_PRINTLN("serial println called");
  SBUDNIC_DEBUG_PRINTLN("done with serial transmit");
  return 0;
}

int SerialLink::transmit(byte* data, uint16_t count) {
  SBUDNIC_DEBUG_PRINTLN("beginning serial transmit");
  Serial.write(data, count);
  SBUDNIC_DEBUG_PRINTLN("serial write called");
  SBUDNIC_DEBUG_PRINTLN("done with serial transmit");
  return 0;
}

char SerialLink::receive(byte* command) {
  SBUDNIC_DEBUG_PRINTLN("beginning serial receive");
  int received = Serial.read();
  if (received == -1) {
    return 0;
  }
  return received;
}

//Code for use of RFM96 radio, not used in final SBUDNIC
RFM96Link::RFM96Link() {

  pinMode(RFM96_PWR_PIN, OUTPUT);

  digitalWrite(RFM96_PWR_PIN, LOW);
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
  // CHECK(ax25.begin(CALL_SIGN));
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

int RFM96Link::transmit(char* data) {
  SBUDNIC_DEBUG_PRINTLN("beginning RFM96 transmit");
  // CHECK(ax25.transmit(data, CALL_SIGN));
  SBUDNIC_DEBUG_PRINTLN("RFM96 ax25 transmit called");
  SBUDNIC_DEBUG_PRINTLN("done with RFM96 transmit");
}

char RFM96Link::receive(byte* command) {
  return 0;
}

//Code to support LoRA transmission on RFM96 radio.
RFM96LoRALink::RFM96LoRALink() {

  pinMode(RFM96_PWR_PIN, OUTPUT);

  digitalWrite(RFM96_PWR_PIN, LOW);
  pinMode(RFM96_NSS_PIN, OUTPUT);
  digitalWrite(RFM96_NSS_PIN, HIGH);
}

int RFM96LoRALink::enable() {
  SBUDNIC_DEBUG_PRINTLN("enabling RFM96");
  digitalWrite(RFM96_PWR_PIN, HIGH);
  delay(50);
  SBUDNIC_DEBUG_PRINTLN("RFM96 power turned on");

  SBUDNIC_DEBUG_PRINTLN("RFM96 object created");
  
  CHECK(radio.begin());
  SBUDNIC_DEBUG_PRINTLN("RFM96 begin called");
  CHECK(radio.setOutputPower(RADIO_RF_POWER));
  SBUDNIC_DEBUG_PRINTLN("RFM96 setOutputPower called");
  CHECK(radio.setFrequency(Config::data.frequency));
  SBUDNIC_DEBUG_PRINTLN("RFM96 setFrequency called");
  CHECK(radio.setCurrentLimit(140));
  SBUDNIC_DEBUG_PRINTLN("RFM96 setCurrentLimit called");
  SBUDNIC_DEBUG_PRINTLN("done enabling RFM96");
  return 0;
}

int RFM96LoRALink::disable() {
  SBUDNIC_DEBUG_PRINTLN("disabling RFM96");
  digitalWrite(RFM96_PWR_PIN, LOW);
  SBUDNIC_DEBUG_PRINTLN("RFM96 power turned off");
  SBUDNIC_DEBUG_PRINTLN("done disabling RFM96");
  return 0;
}

int RFM96LoRALink::transmit(char* data) {
  SBUDNIC_DEBUG_PRINTLN("beginning RFM96 transmit");
  CHECK(radio.transmit(data));
  SBUDNIC_DEBUG_PRINTLN("RFM96 transmit called");
  SBUDNIC_DEBUG_PRINTLN("done with RFM96 transmit");
}

int RFM96LoRALink::transmit(byte* data, uint16_t count) {
  SBUDNIC_DEBUG_PRINTLN("beginning RFM96 transmit");
  CHECK(radio.transmit(data, count));
  SBUDNIC_DEBUG_PRINTLN("RFM96 transmit called");
  SBUDNIC_DEBUG_PRINTLN("done with RFM96 transmit");
}

char RFM96LoRALink::receive(byte* command) {
  SBUDNIC_DEBUG_PRINTLN("Listening");
  int state = radio.receive(command, 256);
  SBUDNIC_DEBUG_PRINTLN(state);
  if (state == RADIOLIB_ERR_NONE) {
    SBUDNIC_DEBUG_PRINTLN((char*)command);
    return 1;
  }
  return 1;
}
