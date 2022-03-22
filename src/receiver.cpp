#include "receiver.h"
#include "pins.h"
#include "buildopt.h"
#include "config.h"



//TODO: how can we tell how big our buffer will need to be?
//  "Each packet is 256 bytes with 205 or 237 bytes of the actual image depending on whether Reed-Solomon forward error correction is used or not."

//TODO: ID (0-255) should change with each photo so we know which packets belong to which photo



int Receiver::enable() {
  char type = SSDV_TYPE_NORMAL;
  int droptest = 0;
  int verbose = 0;
  int errors;
  char callsign[] = "BROWNU";
  int8_t quality = 3;
  SBUDNIC_DEBUG_PRINTLN("starting SSDV setup");
  
  ssdv_enc_init(&ssdv, type, callsign, photoID, quality);
  ssdv_enc_set_buffer(&ssdv, pkt);
  SBUDNIC_DEBUG_PRINTLN("finished SSDV setup");
  
  Receiver::output_position = 0;
  pinMode(UNO_PWR_PIN, OUTPUT);
  digitalWrite(UNO_PWR_PIN, LOW);
  SBUDNIC_DEBUG_PRINTLN("Done enabling Uno");

  Receiver::state = REC_WAIT;
}

int Receiver::disable() {
  SBUDNIC_DEBUG_PRINTLN("Uno disable called");
  pinMode(UNO_PWR_PIN, OUTPUT);
  digitalWrite(UNO_PWR_PIN, HIGH);
  SBUDNIC_DEBUG_PRINTLN("Done disabling uno");
  Receiver::state = REC_OFF;
}

int Receiver::read_data(uint32_t length, uint8_t *jpeg_buffer, uint8_t *output_buffer) {
  Receiver::output_buffer = output_buffer;
}



void Receiver::receive_data(int howMany) {

  if (Receiver::state == REC_DONE) {
    return;
  }
  // SBUDNIC_DEBUG_PRINT("recieving data from uno");
  // SBUDNIC_DEBUG_PRINTLN(howMany);
  uint8_t in[howMany];
  for (int i = 0; i < howMany; i++) {
    in[i] = Wire.read();
  }
  SBUDNIC_DEBUG_PRINT(in[howMany-1]);
  // SBUDNIC_DEBUG_PRINT(" ");
  SBUDNIC_DEBUG_PRINTLN(in[howMany-2]);


  ///////////////////////////////
  int c;
  if((c = ssdv_enc_get_packet(&Receiver::ssdv)) == SSDV_FEED_ME) {

    ssdv_enc_feed(&Receiver::ssdv, in, howMany);

    if((c = ssdv_enc_get_packet(&Receiver::ssdv)) == SSDV_OK) {
      int z;
      for(z=0;z<SSDV_PKT_SIZE;z++){
        // -> put in some external buffer for the radio to access
        Receiver::output_buffer[Receiver::output_position] = Receiver::pkt[z];
        Receiver::output_position = Receiver::output_position + 1;
        if(Receiver::output_position >= Receiver::buf_len) {
          Receiver::state = REC_FULL;
          return;
        }
      }
    }

    if((c = ssdv_enc_get_packet(&Receiver::ssdv)) == SSDV_EOI) {
      Receiver::state = REC_DONE;
      SBUDNIC_DEBUG_PRINT("SSDV DONE");
    }

  } else if (c == SSDV_OK) {
    //in theory shoudlnt come here

    //need to write a packet
    int z;
    for(z=0;z<SSDV_PKT_SIZE;z++){
      // -> put in some external buffer for the radio to access
      Receiver::output_buffer[Receiver::output_position] = Receiver::pkt[z];
      Receiver::output_position = Receiver::output_position + 1;
      SBUDNIC_DEBUG_PRINTLN(output_position);
      if(Receiver::output_position >= Receiver::buf_len) {
        Receiver::state = REC_FULL;
        return;
      }
    }

    if((c = ssdv_enc_get_packet(&Receiver::ssdv)) == SSDV_EOI) {
      Receiver::state = REC_DONE;
      SBUDNIC_DEBUG_PRINT("SSDV DONE");
    }
  } else {
    SBUDNIC_DEBUG_PRINT("SSDV ELSE");
    SBUDNIC_DEBUG_PRINTLN(c);

    //TODO: Something has gone wrong here!
  }
}

void Receiver::send_settings() {
  SBUDNIC_DEBUG_PRINTLN("sending settings");
  Config::load();
  Wire.write(Receiver::useCam1);
  delay(5);
  Wire.write(Config::data.resolution);
}



uint8_t Receiver::get_state() {
  return Receiver::state;
}

int Receiver::get_output_pos() {
  return Receiver::output_position;
}


uint8_t Receiver::pkt[SSDV_PKT_SIZE];
ssdv_t Receiver::ssdv;
int Receiver::buf_len = 0;
int Receiver::photoID = 0;
bool Receiver::useCam1 = false;
uint8_t Receiver::state = REC_OFF;
uint8_t *Receiver::output_buffer;
int Receiver::output_position = 0;

Receiver::Receiver(uint8_t *ob, int blen) {
  Receiver::output_buffer = ob;
  Receiver::buf_len = blen;
  Receiver::photoID = 0;
  Receiver::state = REC_OFF;
  Receiver::useCam1 = false;
  Receiver::output_position = 0;
  pinMode(UNO_PWR_PIN, OUTPUT);
  digitalWrite(UNO_PWR_PIN, HIGH);
  Wire.begin(44);
  Wire.onRequest(Receiver::send_settings);
}