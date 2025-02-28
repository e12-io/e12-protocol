#include "esp32_e12_node_protocol.h"

#include <esp_log.h>

#include "sntp.h"

static const char* TAG = "e12-e32spec";

e12_esp32_node::e12_esp32_node(uint32_t vid, uint32_t pid) : e12(vid, pid) {
  _bus = NULL;
}

e12_esp32_node::~e12_esp32_node() {}
int e12_esp32_node::begin(void* bus, uint8_t e12_addr) {
  ESP_LOGI(TAG, "begin(%x)", bus);
  _bus = (TwoWire*)bus;
  return 0;
}

uint32_t e12_esp32_node::get_time_ms() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_usec / 1000;
}

int e12_esp32_node::send(e12_packet_t* buf) {
  if (!_bus) return -1;
  e12_onwire_t* p = encode(buf);
  if (p) {
    ESP_LOGI(TAG, "E12_OPCODE_WRITE_E12_FRAME (%d: %d)\n", p->data.msg.head.seq,
             p->head.len);
    // for (int i = 0; i < p->head.len; i++) {
    //   ESP_LOGI(TAG, "write (%d:%d:%c)\n", i, p->buf[i], (char)p->buf[i]);
    // }
    _bus->slaveWrite(p->buf, p->head.len);
    return 0;
  }
  return -1;
}

e12_log_evt_t* e12_esp32_node::get_log_evt() { return NULL; }
int e12_esp32_node::on_get_state(char* s, int len, void* ctx) { return 0; }
int e12_esp32_node::on_restore_state(const char* s, int len) { return 0; }
int e12_esp32_node::on_wakeup() { return 0; }
int e12_esp32_node::set_node_auth_credentials(e12_auth_data_t* auth) {
  return 0;
};

int e12_esp32_node::sleep(uint32_t ms, void* data) { return 0; }
int e12_esp32_node::log(uint8_t type, uint8_t status, uint32_t ts, void* data) {
  return 0;
}

int e12_esp32_node::on_config(const char* s, int len) {
  ESP_LOGI(TAG, "e12_esp32_node::on_config()");
  return 0;
}

uint8_t e12_esp32_node::get_checksum(const char* data, uint8_t len) {
  return 0;
}

int e12_esp32_node::print_buffer(e12_onwire_t* buf) {
  ESP_LOGI(TAG, "e12_esp32_node::print()");
  return 0;
}

e12_packet_t* e12_esp32_node::read() {
  if (!_bus) return NULL;

  e12_onwire_t* f = get_decode_buffer();
  flush_buffer(f);
  while (_bus->available()) {
    uint8_t c = _bus->read();
    e12_packet_t* p = NULL;
    if ((p = decode(f, (uint8_t)c))) {
      ESP_LOGI(TAG, "**** Received full e12 FRAME (%d:%d) ****\n",
               p->msg.head.seq, p->msg.head.len);
      // for (int i = 0; i < p->msg.head.len; i++) {
      //   ESP_LOGI(TAG, "Write %d:%d:%c\n", i, p->buf[i], (char)p->buf[i]);
      // }

      while (_bus->available()) _bus->read();
      return p;
    }
  }
  return NULL;
}
