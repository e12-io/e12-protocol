/*
 * Copyright (c) 2023 e12.io
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "esp32_e12_node_protocol.h"

#include <esp_log.h>

#include "sntp.h"

static const char* TAG = "e12-e32spec";

/**
 * @brief Constructor for e12_esp32_node.
 *
 * @param vid Vendor ID.
 * @param pid Product ID.
 */
e12_esp32_node::e12_esp32_node(uint32_t vid, uint32_t pid) : e12(vid, pid) {
  _bus = NULL;
}

/**
 * @brief Destructor for e12_esp32_node.
 */
e12_esp32_node::~e12_esp32_node() {}

/**
 * @brief Initializes the node with the given bus and address.
 *
 * @param bus Pointer to the bus object.
 * @param e12_addr Address of the node.
 * @return int 0 on success, non-zero on failure.
 */
int e12_esp32_node::begin(void* bus, uint8_t e12_addr) {
  ESP_LOGI(TAG, "begin(%x)", bus);
  _bus = (TwoWire*)bus;
  return 0;
}

/**
 * @brief Gets the current time in milliseconds.
 *
 * @return uint32_t Current time in milliseconds.
 */
uint32_t e12_esp32_node::get_time_ms() {
  struct timeval tv;
  if (gettimeofday(&tv, NULL) != 0) {
    ESP_LOGE(TAG, "Failed to get time of day");
    return 0;
  }
  return tv.tv_usec / 1000;
}

/**
 * @brief Sends a packet over the bus.
 *
 * @param buf Pointer to the packet buffer.
 * @return int 0 on success, non-zero on failure.
 */
int e12_esp32_node::send(e12_packet_t* buf) {
  if (!_bus) return -1;
  e12_onwire_t* p = encode(buf);
  if (!p) {
    ESP_LOGE(TAG, "Failed to encode buffer");
    return -1;
  }

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

/**
 * @brief Gets the log event.
 *
 * @return e12_log_evt_t* Pointer to the log event.
 */
e12_log_evt_t* e12_esp32_node::get_log_evt() { return NULL; }

/**
 * @brief Callback for getting the state.
 *
 * @param s Pointer to the state string.
 * @param len Length of the state string.
 * @param ctx Context pointer.
 * @return int 0 on success, non-zero on failure.
 */
int e12_esp32_node::on_get_state(char* s, int len, void* ctx) {
  ESP_LOGI(TAG, "e12_esp32_node::on_get_state()");
  return 0;
}

/**
 * @brief Callback for restoring the state.
 *
 * @param s Pointer to the state string.
 * @param len Length of the state string.
 * @return int 0 on success, non-zero on failure.
 */
int e12_esp32_node::on_restore_state(const char* s, int len) {
  ESP_LOGI(TAG, "e12_esp32_node::on_restore_state()");
  return 0;
}

/**
 * @brief Callback for waking up the node.
 *
 * @return int 0 on success, non-zero on failure.
 */
int e12_esp32_node::on_wakeup() {
  ESP_LOGI(TAG, "e12_esp32_node::on_wakeup()");
  return 0;
}

/**
 * @brief Sets the node authentication credentials.
 *
 * @param auth Pointer to the authentication data.
 * @return int 0 on success, non-zero on failure.
 */
int e12_esp32_node::set_node_auth_credentials(e12_auth_data_t* auth) {
  ESP_LOGI(TAG, "e12_esp32_node::set_node_auth_credentials()");
  return 0;
}

/**
 * @brief Puts the node to sleep for the specified duration.
 *
 * @param ms Duration in milliseconds.
 * @param data Pointer to additional data.
 * @return int 0 on success, non-zero on failure.
 */
int e12_esp32_node::sleep(uint32_t ms, void* data) {
  ESP_LOGI(TAG, "e12_esp32_node::sleep()");
  return 0;
}

/**
 * @brief Logs an event.
 *
 * @param type Type of the event.
 * @param status Status of the event.
 * @param ts Timestamp of the event.
 * @param data Pointer to additional data.
 * @return int 0 on success, non-zero on failure.
 */
int e12_esp32_node::log(uint8_t type, uint8_t status, uint32_t ts, void* data) {
  ESP_LOGI(TAG, "e12_esp32_node::log()");
  return 0;
}

/**
 * @brief Callback for configuring the node.
 *
 * @param s Pointer to the configuration string.
 * @param len Length of the configuration string.
 * @return int 0 on success, non-zero on failure.
 */
int e12_esp32_node::on_config(const char* s, int len) {
  ESP_LOGI(TAG, "e12_esp32_node::on_config()");
  return 0;
}

/**
 * @brief Prints the content of the buffer.
 *
 * @param buf Pointer to the buffer.
 * @return int 0 on success, non-zero on failure.
 */
int e12_esp32_node::print_buffer(e12_onwire_t* buf) {
  if (!buf) {
    ESP_LOGE(TAG, "Buffer is NULL");
    return -1;
  }
  ESP_LOGI(TAG, "e12_esp32_node::print()");
  // Add code to print buffer content if needed
  return 0;
}

/**
 * @brief Reads a packet from the bus.
 *
 * @return e12_packet_t* Pointer to the read packet, or NULL on failure.
 */
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
