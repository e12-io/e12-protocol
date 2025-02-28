#ifndef ARDUINO_E12_SPEC_H
#define ARDUINO_E12_SPEC_H

#include <Wire.h>
#include <e12_protocol.h>
#include <stdint.h>

#define DEBUG 1

typedef struct e12_event {
  void* data;
} e12_event_t;

class e12_arduino : public e12 {
 private:
  uint8_t _e12_addr;
  TwoWire* _bus;
  uint32_t _evt_count;
  e12_log_evt_t _log[E12_MAX_LOG_BUFFERS];

 public:
  e12_arduino(uint32_t vid, uint32_t pid);
  ~e12_arduino();

  void e12_run();
  int close();

  virtual int begin(void* bus, uint8_t e12_addr = 0);
  virtual uint32_t get_time_ms();
  virtual int send(e12_packet_t* buf);
  virtual e12_packet_t* read();
  virtual e12_log_evt_t* get_log_evt();
  virtual int set_node_auth_credentials(e12_auth_data_t* auth);
  virtual int on_wakeup();
  virtual int sleep(uint32_t ms, void* data);
  virtual uint8_t get_checksum(const char* data, uint8_t len);
};

#endif
