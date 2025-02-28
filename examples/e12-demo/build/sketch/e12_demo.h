#line 1 "D:\\work\\e12-io\\e12-firmware-esp32\\lib\\e12-protocol\\examples\\e12-demo\\e12_demo.h"
#ifndef H_E12_DEMO
#define H_E12_DEMO

#include <DallasTemperature.h>
#include <Wire.h>

#include "Arduino.h"
#include "e12_client.h"
//
#include <e12_protocol.h>
//

#include "Arduino.h"
#include "arduino/arduino_e12_protocol.h"

enum {
  EVT_BLINK = 0x01,
  EVT_TEMP = 0x02,
  EVT_DEMO = 0x03,
} event_t;

class e12_demo : public e12_client {
 private:
  uint8_t _on;
  uint32_t _on_delay;
  uint32_t _off_delay;
  uint8_t _pin;
  uint8_t _count;
  float _temp;

 public:
  e12_demo(uint32_t vid, uint32_t pid);
  ~e12_demo() {}
  int on_config(const char* s, int len);
  int on_get_state(char* s, int len, void* ctx);
  int on_restore_state(const char* s, int len);
  int log(uint8_t type, uint8_t status, uint32_t ts, void* data);

  // client device functions
  uint32_t blink();
  uint32_t read_temp(DallasTemperature* sensors);
  uint32_t demo();
};

#endif
