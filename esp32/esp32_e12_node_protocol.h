#ifndef H_ESP32_E12_SPEC
#define H_ESP32_E12_SPEC
#include <Wire.h>
#include <e12_protocol.h>
#include <stdint.h>

// This esp e12 node.
class e12_esp32_node : public e12 {
 private:
  TwoWire* _bus;

 public:
  e12_esp32_node(uint32_t vid, uint32_t pid);

  ~e12_esp32_node();

  virtual int begin(void* bus, uint8_t e12_addr = 0);
  virtual uint32_t get_time_ms();
  virtual int send(e12_packet_t* buf);
  virtual e12_packet_t* read();

  virtual int print_buffer(e12_onwire_t* buf);
  virtual uint8_t get_checksum(const char* data, uint8_t len);
  virtual e12_log_evt_t* get_log_evt();
  virtual int set_node_auth_credentials(e12_auth_data_t* auth);

  virtual int on_config(const char* s, int len);
  virtual int on_get_state(char* s, int len, void* ctx);
  virtual int on_restore_state(const char* s, int len);
  virtual int on_wakeup();
  virtual int sleep(uint32_t ms, void* data);
  virtual int log(uint8_t type, uint8_t status, uint32_t ts, void* data);
};

#endif