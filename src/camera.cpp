#include "camera.h"
#include "pins.h"
#include "buildopt.h"

/**
 * how i think  this all will  be used in our main loop
 * camera.enable()
 * l = camera.takepicture
 * outputbuffer[<however big we will need for the packets>]
 * camera.readdata(outputbuffer, <that specified size>)
 * 
 **/


//TODO: how can we tell how big our buffer will need to be?
//  "Each packet is 256 bytes with 205 or 237 bytes of the actual image depending on whether Reed-Solomon forward error correction is used or not."

//TODO: ID (0-255) should change with each photo so we know which packets belong to which photo


// ArduCAM cam1(OV5642, CAM1_CS_PIN);
// ArduCAM cam2(OV5642, CAM2_CS_PIN);

int Camera::enable() {
  
  //Mosfet open
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, LOW);

  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);

  delay(500);
  if(powerPin == CAM1_PWR_PIN){
    camera = ArduCAM(OV5642, CAM1_CS_PIN);
  }else{
    camera = ArduCAM(OV5642, CAM2_CS_PIN);
  }
  Wire.begin();
  // initialize SPI:
  SPI.begin();

  SBUDNIC_DEBUG_PRINTLN("Camera SPI interface intialized");
  //Reset the CPLD
  camera.write_reg(0x07, 0x80);
  delay(100);
  camera.write_reg(0x07, 0x00);
  delay(100);
  SBUDNIC_DEBUG_PRINTLN("Camera CPLD reset");

  while (1) {
  //Check if the ArduCAM SPI bus is OK
    camera.write_reg(ARDUCHIP_TEST1, 0x55);
    uint8_t temp = camera.read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55) {
      SBUDNIC_DEBUG_PRINTLN(F("Camera SPI interface error"));
      delay(1000);
      continue;
    } else {
      SBUDNIC_DEBUG_PRINTLN(F("Camera SPI interface okay"));
      break;
    }
  }

  camera.set_format(JPEG);
  SBUDNIC_DEBUG_PRINTLN("Camera set to JPEG format");
  camera.InitCAM();
  SBUDNIC_DEBUG_PRINTLN("Camera InitCAM called");

  camera.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
  camera.OV5642_set_JPEG_size(OV5642_320x240);

  delay(1000);
  camera.clear_fifo_flag();
  camera.write_reg(ARDUCHIP_FRAMES, 0x00);
  SBUDNIC_DEBUG_PRINTLN("Done enabling camera");
  camera.clear_fifo_flag();
}

int Camera::disable() {
  SBUDNIC_DEBUG_PRINTLN("Camera disable called");
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);
  digitalWrite(csPin, HIGH);
  SBUDNIC_DEBUG_PRINTLN("Done disabling camera");
}

// //Function which takes in a buffer address and reads bytes from the camera buffer into it.
// //returns: the number of bits read to the buffer.
// size_t Camera::populate_buffer(uint8_t *b)
// {
//   uint8_t temp = 0, temp_last = 0;
//   uint32_t length = 0;
//   bool is_header = false;
//   length = camera.read_fifo_length();

//   camera.CS_LOW();
//   camera.set_fifo_burst();
//   temp = SPI.transfer(0x00);
//   length--;

//   int ptr = 0;

//   while ( length-- )
//   {
//     temp_last = temp;
//     temp =  SPI.transfer(0x00);
//     if (is_header == true)
//     {
//       b[ptr] = temp;
//       ptr = ptr + 1;
//     }
//     else if ((temp == 0xD8) & (temp_last == 0xFF))
//     {
//       is_header = true;
//       b[ptr] = temp_last;
//       b[ptr+1] = temp;

//       //shouldnt matter...
//       ptr = ptr + 2;
//     }
//     if ( (temp == 0xD9) && (temp_last == 0xFF) )
//     break;
//     delayMicroseconds(15);
//   }
//   camera.CS_HIGH();
//   is_header = false;
//   return (ptr);
// }

//Function which takes in a pointer to the full buffer and packetizes/writes them to a buffer.
//returns: number of packets made

//TODO: length argument is not needed most likely
int Camera::transmit_packets(uint32_t length, uint8_t *jpeg_buffer, uint8_t *output_buffer) {
///////////////////////
  uint8_t full_buffer[128] = {0};
  uint8_t temp = 0, temp_last = 0;
  bool is_header = false;

  SBUDNIC_DEBUG_PRINTLN("starting to read from camera buffer");

  camera.CS_LOW();
  delay(100);
  camera.set_fifo_burst();
  temp = SPI.transfer(0x00);
///////////////////////

  SBUDNIC_DEBUG_PRINTLN("starting ssdv encoding");



  // int ptr = 0;
  int c;
  int pkt_cnt = 0;
  int output_position = 0;
      
  while(1){
    while((c = ssdv_enc_get_packet(&ssdv)) == SSDV_FEED_ME){
      SBUDNIC_DEBUG_PRINTLN("ssdv encoding");

      //////////////////////
      //Get the next <=128 bytes from the photo
      int ptr = 0;
      while(ptr < 128){
        temp_last = temp;
        temp =  SPI.transfer(0x00);
        if (is_header == true){
          full_buffer[ptr] = temp;
          ptr = ptr + 1;
        }
        else if ((temp == 0xD8) & (temp_last == 0xFF)){
          is_header = true;
          full_buffer[ptr] = temp_last;
          full_buffer[ptr+1] = temp;

          ptr = ptr + 2;
        }
        if ((temp == 0xD9) && (temp_last == 0xFF)) {
          break;
        }
        delayMicroseconds(15);
      }


      //////////////////////  


      
      
      //encode the next chunk of data
      ssdv_enc_feed(&ssdv, full_buffer, ptr);
    }
        
    if(c == SSDV_EOI){
      //if done reading the buffer
      break;
    } else if(c != SSDV_OK){
      //if something breaks
      SBUDNIC_DEBUG_PRINTLN("Camera transmit packets error");
      SBUDNIC_DEBUG_PRINTLN(c);
      return -1;
    }
        
    int z;
    for(z=0;z<SSDV_PKT_SIZE;z++){

      // -> put in some external buffer for the radio to access
      output_buffer[output_position] = pkt[z];
      output_position = output_position + 1;
    }

    ///POSSIBLE TO CHANGE THE ABOVE LOOP TO FOLLOWING CODE - TEST THIS
    // std::memcpy(&(output_buffer[pkt_cnt*SSDV_PKT_SIZE]), pkt, SSDV_PKT_SIZE)

    pkt_cnt = pkt_cnt + 1;
  }

  camera.CS_HIGH();
  camera.clear_fifo_flag();
  SBUDNIC_DEBUG_PRINTLN("Camera transmit packets done");
  
  return pkt_cnt;
}


uint32_t Camera::takePicture() {
  camera.flush_fifo();
  camera.clear_fifo_flag();
  camera.start_capture();

  while(!camera.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));

  uint32_t length = camera.read_fifo_length();
  SBUDNIC_DEBUG_PRINTLN("Picture taken with length ");
  SBUDNIC_DEBUG_PRINTLN(length);
  jpegLen = length;
  return length;
}


//"data" is the buffer where all the packets will be stored in order
// size is probably gonna be the length of the buffer, currently not used 
//TODO: use size to make sure the buffer is big enough to write all our packets in

//returns number of packets written
int Camera::readData(uint8_t* data, size_t size, int photoId) {
  
  uint8_t full_buffer[1] = {0};
  size_t l = 0;
  //here, full_buffer has the entire jpeg image
  char type = SSDV_TYPE_NORMAL;
  int droptest = 0;
  int verbose = 0;
  int errors;
  char callsign[] = "BROWNU";
  int8_t quality = 4;

  SBUDNIC_DEBUG_PRINTLN("starting SSDV setup");
  
  ssdv_enc_init(&ssdv, type, callsign, photoId, quality);
  ssdv_enc_set_buffer(&ssdv, pkt);
  SBUDNIC_DEBUG_PRINTLN("finished SSDV setup");
  
  return transmit_packets(l, full_buffer, data);
  //all the packets will be written to the 'data' buffer
}


Camera::Camera(uint16_t id) {
  if (id == 0) {
    powerPin = CAM1_PWR_PIN;
    csPin = CAM1_CS_PIN;
  } else {
    powerPin = CAM2_PWR_PIN;
    csPin = CAM2_CS_PIN;
  }
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);
}
