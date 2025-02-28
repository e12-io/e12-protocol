#line 1 "D:\\work\\e12-io\\e12-firmware-esp32\\lib\\e12-protocol\\examples\\e12-demo\\e12_client.h"
#ifndef H_E12_CLIENT
#define H_E12_CLIENT

#include <Wire.h>

#include "Arduino.h"
//
#include <e12_protocol.h>

#include "arduino/arduino_e12_protocol.h"

class e12_client : public e12_arduino {
 private:
 public:
  e12_client(uint32_t vid, uint32_t pid) : e12_arduino(vid, pid) {}

  ~e12_client() {}
  int wakeup_e12();
  int sleep(uint32_t ms, void* data);
  int send(e12_packet_t* buf);
  int on_receive(e12_packet_t* p);
};

#endif
