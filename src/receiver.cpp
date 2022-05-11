#include "receiver.h"
#include "pins.h"
#include "buildopt.h"
#include "config.h"

//This code is utilized by the BLE to communicate with the Mega and process the data.

/**
 * @brief Method to being the photo processing process. Turn on the Mega, 
 * intialize the SSDV encoder.
 * 
 * @return int corresponding to status of the Mega.
 */
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

/**
 * @brief Method to turn off the Mega
 */
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

/**
 * @brief Method to recieve the image data from the Mega and packetize it using 
 * SSDV
 * 
 * @param howMany the max number of bytes we can store in our output buffer.
 */
void Receiver::receive_data(int howMany) {

  if (Receiver::state == REC_DONE) {
    return;
  }

  // Read in the max allowable data from the Mega
  uint8_t in[howMany];
  for (int i = 0; i < howMany; i++) {
    in[i] = Wire.read();
  }
  SBUDNIC_DEBUG_PRINT(in[howMany-1]);
  // SBUDNIC_DEBUG_PRINT(" ");
  SBUDNIC_DEBUG_PRINTLN(in[howMany-2]);

  //Encode the image into SSDV packets
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
    Receiver::state = REC_FAIL;
  }
}

/**
 * @brief Method which transmits camera configuration to Mega
 * 
 */
void Receiver::send_settings() {
  SBUDNIC_DEBUG_PRINTLN("sending settings");
  Config::load();
  Wire.write(Receiver::useCam1);
  delay(5);
  Wire.write(Config::data.resolution);
}

//Check the state of the Mega
uint8_t Receiver::get_state() {
  return Receiver::state;
}

//Get the current position in the image buffer that the Mega is transmitting
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

/**
 * @brief Construct a new Receiver:: Receiver object
 * 
 * @param ob the output buffer which will store the SSDV packets
 * @param blen the length of the output buffer
 */
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