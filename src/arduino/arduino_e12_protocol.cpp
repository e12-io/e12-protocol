#include "arduino_e12_protocol.h"

#include <stdint.h>

#include "Arduino.h"
#include "e12_variants.h"

#define MAX_RESP_TIMEOUT 5000
#define DEBUG 1

e12_arduino::e12_arduino(uint32_t vid, uint32_t pid) : e12(vid, pid) {}

e12_arduino::~e12_arduino() {}

int e12_arduino::begin(void* bus, uint8_t e12_addr) {
  _bus = (TwoWire*)bus;
  _e12_addr = e12_addr;
  _timeout = MAX_RESP_TIMEOUT;
  _evt_count = 0;

#ifdef ARDUINO_SAMD_ZERO  //__SAMD21__
  // default is 20, 21
#else
  _bus->setSDA(20);
  _bus->setSCL(21);
#endif

  _bus->begin();
  _bus->setClock(100000UL);  // experimenting with 1Mhz speed

  // good time to make sure that e12 node is awake
  set_node_status(e12_node_op_status_t::STATUS_ACTIVE, 0);
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
  // on wake up, restore your config and state
  // from e12 node
  send(get_request(e12_cmd_t::CMD_CONFIG));
  send(get_request(e12_cmd_t::CMD_STATE));
  return 0;
}

int e12_arduino::sleep(uint32_t ms, void* data) {
  if (ms) {
    // Before going to sleep, inform the e12 node
    // to schedule a wakeup interrupt on the configured pin
    e12_wakeup_data_t wakeup = {0};
    wakeup.ms = ms;
    send(get_request(e12_cmd_t::CMD_SCHEDULE_WAKEUP, true, (void*)&wakeup));
  }
  return 0;
}

int e12_arduino::send(e12_packet_t* buf, bool retry) {
  if (!buf) return 0;
  if (get_node_status() == e12_node_op_status_t::STATUS_SLEEP) {
    set_node_status(e12_node_op_status_t::STATUS_ACTIVE, 0);
    if (retry) {
      // typically e12-node should wake up and become
      // operational in less than 300ms
      delay(500);
    } else {
      return (int)e12_err_t::ERR_RETRY_LATER;
    }
  }

  e12_onwire_t* req = encode(buf);
  req->resp_pending = req->data.msg.head.RESP_EXPECTED;
  req->ts = millis();

  _bus->beginTransmission(_e12_addr);

#if 0
  Serial.print("Sending Request cmd: ");
  Serial.println((int)buf->msg.head.cmd);
  for (int i = 0; i < req->head.len; i++) {
    uint8_t c = req->buf[i];
    Serial.print((byte)c);
  }
  Serial.println("");
#endif

  _bus->write(req->buf, req->head.len);
  if (_bus->endTransmission() != 0) {
    return -1;
  }
  return req->head.len;
}

e12_packet_t* e12_arduino::read() {
  int num = _bus->requestFrom(_e12_addr, (uint8_t)sizeof(e12_onwire_t));
#if 0
  Serial.print("e12_arduino::read() : ");
  Serial.println(num);
#endif
  if (!num) return NULL;

  e12_onwire_t* f = get_decode_buffer();
  flush_buffer(f);
  e12_packet_t* p = NULL;
  while (_bus->available()) {
    uint8_t c = _bus->read();
#if 0
    Serial.print((uint8_t)c);
#endif
    if ((p = decode(f, c))) {
      while (_bus->available()) _bus->read();
      return p;
    }
  }
  return NULL;
}

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
