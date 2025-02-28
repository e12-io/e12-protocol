#include "arduino_e12_protocol.h"

#include <stdint.h>

#include "Arduino.h"

#define MAX_RESP_TIMEOUT 5000
#define E12_INTR_PIN 2

e12_arduino::e12_arduino(uint32_t vid, uint32_t pid) : e12(vid, pid) {}

e12_arduino::~e12_arduino() {}

int e12_arduino::begin(void* bus, uint8_t e12_addr) {
  _bus = (TwoWire*)bus;
  _e12_addr = e12_addr;
  _timeout = MAX_RESP_TIMEOUT;
  _evt_count = 0;

  _bus->setSDA(20);
  _bus->setSCL(21);

  _bus->begin();
  // _bus->setClock(100000);
  _bus->setClock(100000UL);  // experimenting with 1Mhz speed
  return 0;
}

int e12_arduino::close() {
  _bus->end();
  return 0;
}

uint32_t e12_arduino::get_time_ms() { return millis(); }
int e12_arduino::set_node_auth_credentials(e12_auth_data_t* auth) {
  if (!auth) return -1;
  send(get_request(e12_cmd_t::CMD_AUTH, true, (void*)auth));
  return 0;
}

int e12_arduino::on_wakeup() {
  // do a synchronous send req/read response
  // get CONFIG and STATE
  send(get_request(e12_cmd_t::CMD_CONFIG));
  send(get_request(e12_cmd_t::CMD_STATE));
  return 0;
}

int e12_arduino::sleep(uint32_t ms, void* data) {
  if (ms) {
    Serial.println("Sending WAKE ME UP --->");
    e12_wakeup_data_t wakeup = {0};
    wakeup.ms = ms;
    send(get_request(e12_cmd_t::CMD_WAKE_ME_UP, true, (void*)&wakeup));
    if (!read()) {
      wakeup_e12();
      return -1;
    }
    send(get_request(e12_cmd_t::CMD_STATE, true, (void*)true));
    read();
  }
  return 0;
}

int e12_arduino::send(e12_packet_t* buf) {
  if (!buf) return 0;

  e12_onwire_t* req = encode(buf);
  req->resp_pending = req->data.msg.head.RESP_EXPECTED;
  req->ts = millis();

  _bus->beginTransmission(_e12_addr);
#if DEBUG
  for (int i = 0; i < req->head.len; i++) {
    uint8_t c = req->buf[i];
    Serial.print((byte)c);
  }
  Serial.println("");
#endif
  _bus->write(req->buf, req->head.len);
  _bus->endTransmission();
  return req->head.len;
}

e12_packet_t* e12_arduino::read() {
  int num = _bus->requestFrom(_e12_addr, (uint8_t)sizeof(e12_onwire_t));
  Serial.print("e12_arduino::read() : ");
  Serial.println(num);
  if (!num) return NULL;

  e12_onwire_t* f = get_decode_buffer();
  flush_buffer(f);
  e12_packet_t* p = NULL;
  while (_bus->available()) {
    uint8_t c = _bus->read();
#if DEBUG
    Serial.print((unsigned byte)c);
#endif
    if ((p = decode(f, (uint8_t)c))) {
      while (_bus->available()) _bus->read();
      return p;
    }
  }
  return NULL;
}

uint8_t e12_arduino::get_checksum(const char* data, uint8_t len) { return 0; }
void e12_arduino::e12_run() {
  if (!is_configured()) {
    send(get_request(e12_cmd_t::CMD_CONFIG));
  }
}

e12_log_evt_t* e12_arduino::get_log_evt() {
  for (int i = 0; i < E12_MAX_LOG_BUFFERS; i++) {
    if (!_log[i].in_use) {
      memset(&_log[i], 0, sizeof(e12_log_evt_t));
      _log[i].in_use = true;
      _log[i].count = _evt_count++;
      return &_log[i];
    }
  }
  return NULL;
}
