#include "reciever.h"
#include "pins.h"
#include "buildopt.h"



//TODO: how can we tell how big our buffer will need to be?
//  "Each packet is 256 bytes with 205 or 237 bytes of the actual image depending on whether Reed-Solomon forward error correction is used or not."

//TODO: ID (0-255) should change with each photo so we know which packets belong to which photo



int Reciever::enable() {
  char type = SSDV_TYPE_NORMAL;
  int droptest = 0;
  int verbose = 0;
  int errors;
  char callsign[] = "BROWNU";
  int8_t quality = 4;
  SBUDNIC_DEBUG_PRINTLN("starting SSDV setup");
  
  ssdv_enc_init(&ssdv, type, callsign, photoID, quality);
  ssdv_enc_set_buffer(&ssdv, pkt);
  SBUDNIC_DEBUG_PRINTLN("finished SSDV setup");
  
  Reciever::output_position = 0;
  pinMode(UNO_PWR_PIN, OUTPUT);
  digitalWrite(UNO_PWR_PIN, LOW);
  SBUDNIC_DEBUG_PRINTLN("Done enabling Uno");

  Reciever::state = REC_WAIT;
}

int Reciever::disable() {
  SBUDNIC_DEBUG_PRINTLN("Uno disable called");
  pinMode(UNO_PWR_PIN, OUTPUT);
  digitalWrite(UNO_PWR_PIN, HIGH);
  SBUDNIC_DEBUG_PRINTLN("Done disabling uno");
  Reciever::state = REC_OFF;
}

int Reciever::read_data(uint32_t length, uint8_t *jpeg_buffer, uint8_t *output_buffer) {
  Reciever::output_buffer = output_buffer;
}



void Reciever::recieve_data(int howMany) {

  if (Reciever::state == REC_DONE) {
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
  if((c = ssdv_enc_get_packet(&Reciever::ssdv)) == SSDV_FEED_ME) {

    ssdv_enc_feed(&Reciever::ssdv, in, howMany);

    if((c = ssdv_enc_get_packet(&Reciever::ssdv)) == SSDV_OK) {
      int z;
      for(z=0;z<SSDV_PKT_SIZE;z++){
        // -> put in some external buffer for the radio to access
        Reciever::output_buffer[Reciever::output_position] = Reciever::pkt[z];
        Reciever::output_position = Reciever::output_position + 1;
        if(Reciever::output_position >= Reciever::buf_len) {
          Reciever::state = REC_FULL;
          return;
        }
      }
    }

    if((c = ssdv_enc_get_packet(&Reciever::ssdv)) == SSDV_EOI) {
      Reciever::state = REC_DONE;
    SBUDNIC_DEBUG_PRINT("SSDV DONE");
    }

  } else if (c == SSDV_OK) {
    //in theory shoudlnt come here

    //need to write a packet
    int z;
    for(z=0;z<SSDV_PKT_SIZE;z++){
      // -> put in some external buffer for the radio to access
      Reciever::output_buffer[Reciever::output_position] = Reciever::pkt[z];
      Reciever::output_position = Reciever::output_position + 1;
      SBUDNIC_DEBUG_PRINTLN(output_position);
      if(Reciever::output_position >= Reciever::buf_len) {
        Reciever::state = REC_FULL;
        return;
      }
    }

    if((c = ssdv_enc_get_packet(&Reciever::ssdv)) == SSDV_EOI) {
      Reciever::state = REC_DONE;
    SBUDNIC_DEBUG_PRINT("SSDV DONE");
    }
  } else {
    SBUDNIC_DEBUG_PRINT("SSDV ELSE");
    SBUDNIC_DEBUG_PRINTLN(c);
  }
  
///////////////////////////////

  // if((c = ssdv_enc_get_packet(&Reciever::ssdv)) != SSDV_FEED_ME) {
  //   if(c == SSDV_EOI){
	// 		// SBUDNIC_DEBUG_PRINTLN("SSDV EOI");
	// 		//return here/set state to done
  //     Reciever::state = REC_DONE;
  //     return;
	// 	}else if(c != SSDV_OK){
	// 		SBUDNIC_DEBUG_PRINTLN("SSDV NOT OK");
	// 		Reciever::state = REC_FULL;
  //     return;
	// 	}

  //   int z;
  //   SBUDNIC_DEBUG_PRINTLN("writing packet");
  //   for(z=0;z<SSDV_PKT_SIZE;z++){
  //     // -> put in some external buffer for the radio to access
  //     Reciever::output_buffer[Reciever::output_position] = Reciever::pkt[z];
  //     Reciever::output_position = Reciever::output_position + 1;

  //     if(Reciever::output_position >= Reciever::buf_len) {
  //       Reciever::state = REC_FULL;
  //       return;
  //     }
  //   }
  //   SBUDNIC_DEBUG_PRINTLN(Reciever::output_position);
  // }

  // if((c = ssdv_enc_get_packet(&Reciever::ssdv)) == SSDV_FEED_ME) {
  //   ssdv_enc_feed(&Reciever::ssdv, in, howMany);
  // }

  // if(in[howMany-1]==217 && in[howMany-2]==255) {
  //   Reciever::state = REC_DONE;
  //   // SBUDNIC_DEBUG_PRINTLN("should end");
  // }

  // delay(100);


///////////////////////////

// while(1)
// 		{
// 			while((c = ssdv_enc_get_packet(&ssdv)) == SSDV_FEED_ME)
// 			{
// 				size_t r = fread(b, 1, 128, fin);
				
// 				if(r <= 0)
// 				{
// 					fprintf(stderr, "Premature end of file\n");
// 					break;
// 				}
// 				ssdv_enc_feed(&ssdv, b, r);
// 			}
			
// 			if(c == SSDV_EOI)
// 			{
// 				fprintf(stderr, "ssdv_enc_get_packet said EOI\n");
// 				break;
// 			}
// 			else if(c != SSDV_OK)
// 			{
// 				fprintf(stderr, "ssdv_enc_get_packet failed: %i\n", c);
// 				return(-1);
// 			}
			
// 			fwrite(pkt, 1, SSDV_PKT_SIZE, fout);
// 			i++;
// 		}



// ////////////////////////////




//   ssdv_enc_feed(&Reciever::ssdv, in, howMany);

//   int c = ssdv_enc_get_packet(&Reciever::ssdv);


  

//   if(c != SSDV_FEED_ME){

//     if(c != SSDV_OK){
//       //if something breaks
//       SBUDNIC_DEBUG_PRINTLN("Camera transmit packets error");
//       return;
//     }

//     //have to deal with completed packet here
//     int z;
//     SBUDNIC_DEBUG_PRINTLN("writing packet");
//     for(z=0;z<SSDV_PKT_SIZE;z++){
//       // -> put in some external buffer for the radio to access
//       Reciever::output_buffer[Reciever::output_position] = Reciever::pkt[z];
//       Reciever::output_position = Reciever::output_position + 1;

//       if(Reciever::output_position >= Reciever::buf_len) {
//         Reciever::state = REC_FULL;
//         return;
//       }
//     }
//   }

//   if(c == SSDV_EOI) {
//     SBUDNIC_DEBUG_PRINTLN("packeting image done");
//     Reciever::state = REC_DONE;
//   }
//   SBUDNIC_DEBUG_PRINTLN(c);
}

void Reciever::send_settings() {
  Wire.write(Reciever::useCam1);
}



uint8_t Reciever::get_state() {
  return Reciever::state;
}

int Reciever::get_output_pos() {
  return Reciever::output_position;
}


uint8_t Reciever::pkt[SSDV_PKT_SIZE];
ssdv_t Reciever::ssdv;
int Reciever::buf_len = 0;
int Reciever::photoID = 0;
bool Reciever::useCam1 = false;
uint8_t Reciever::state = REC_OFF;
uint8_t *Reciever::output_buffer;
int Reciever::output_position = 0;

Reciever::Reciever(uint8_t *ob, int blen) {
  Reciever::output_buffer = ob;
  Reciever::buf_len = blen;
  Reciever::photoID = 0;
  Reciever::state = REC_OFF;
  Reciever::useCam1 = false;
  Reciever::output_position = 0;
  pinMode(UNO_PWR_PIN, OUTPUT);
  digitalWrite(UNO_PWR_PIN, HIGH);
  Wire.begin(44);
  Wire.onRequest(Reciever::send_settings);
}