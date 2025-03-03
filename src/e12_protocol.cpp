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

#include "e12_protocol.h"

#include <stdint.h>
#include <string.h>

#if ESP32_E12_SPEC
#include <esp_log.h>
static const char* TAG = "e12-spec";
#endif

/**
 * @brief Construct a new e12 object
 *
 * @param vid Vendor ID
 * @param pid Product ID
 */
e12::e12(uint32_t vid, uint32_t pid) {
  _vid = vid;
  _pid = pid;
  _status.CONFIGURED = false;
}

/**
 * @brief Destroy the e12 object
 */
e12::~e12() {}

/**
 * @brief Encode the given data into an on-wire packet
 *
 * @param data Pointer to the data packet to encode
 * @return e12_onwire_t* Pointer to the encoded on-wire packet
 */
e12_onwire_t* e12::encode(e12_packet_t* data) {
  // NOTE: below pointer airthmetics does not work?? why??
  // e12_onwire_t* pkt = (e12_onwire_t*)(data - sizeof(e12_onwire_head_t));
  e12_onwire_t* pkt = get_encode_buffer();
  pkt->head.magic[0] = E12_MAGIC_MARKER_1;  // e12
  pkt->head.magic[1] = E12_MAGIC_MARKER_2;
  pkt->head.len = sizeof(e12_onwire_head_t) + sizeof(e12_header_t) +
                  data->msg.head.len + sizeof(pkt->checksum);
  pkt->checksum = get_checksum((const char*)&pkt->data, sizeof(e12_packet_t));
  return pkt;
}

/**
 * @brief Set the node properties
 *
 * @param props Pointer to the node properties structure
 */
void e12::set_node_properties(e12_node_properties_t* props) {
  send(get_request(e12_cmd_t::CMD_SET_NODE_PROPERTIES, true, (void*)&props));
}

#define STR_PING "ping"
#define STR_PONG "pong"

/**
 * @brief Get a request packet for the given command
 *
 * @param cmd Command type
 * @param response Whether a response is expected
 * @param data Pointer to the data associated with the command
 * @return e12_packet_t* Pointer to the request packet
 */
e12_packet_t* e12::get_request(e12_cmd_t cmd, bool response, void* data) {
  e12_packet_t* p = e12_get_packet();
  if (!p) return NULL;
  p->msg.head.cmd = cmd;
  p->msg.head.RESP_EXPECTED = response;
  p->msg.head.len = 0;
  switch (cmd) {
    case e12_cmd_t::CMD_PING: {
      p->msg.head.len = strlen(STR_PING) + 1;
      memcpy(p->msg.data, STR_PING, p->msg.head.len);
    } break;
    case e12_cmd_t::CMD_DEBUG_BLINK: {
      e12_debug_blink_t* blink = (e12_debug_blink_t*)data;
      p->msg_debug_blink.data.on_ms = blink->on_ms;
      p->msg_debug_blink.data.off_ms = blink->off_ms;
      p->msg_debug_blink.data.count = blink->count;
      p->msg.head.len = sizeof(e12_debug_blink_t);
    } break;
    case e12_cmd_t::CMD_SET_NODE_PROPERTIES: {
      e12_node_properties_t* props = (e12_node_properties_t*)data;
      p->msg_node_props.data.flags = props->flags;
      p->msg.head.len = sizeof(e12_node_properties_t);
    } break;
    case e12_cmd_t::CMD_TIME: {
    } break;
    case e12_cmd_t::CMD_CONFIG: {
    } break;
    case e12_cmd_t::CMD_LOG: {
      e12_log_evt_t* log = (e12_log_evt_t*)data;
      p->msg.head.len = sizeof(e12_log_evt_t);
      memcpy(p->msg.data, data, p->msg.head.len);
    } break;
    case e12_cmd_t::CMD_WAKE_ME_UP: {
      e12_wakeup_data_t* wakeup = (e12_wakeup_data_t*)data;
      p->msg_wakeup.ms = wakeup->ms;
      p->msg.head.len = sizeof(uint32_t);
    } break;
    case e12_cmd_t::CMD_AUTH: {
      e12_auth_data_t* auth = (e12_auth_data_t*)data;
      p->msg.head.len = sizeof(e12_auth_data_t);
      memcpy(p->msg.data, data, p->msg.head.len);
    } break;
    case e12_cmd_t::CMD_STATE: {
      e12_data_t* s = (e12_data_t*)p->msg.data;
      s->IS_JSON = true;
      p->msg.head.len = 1;
      if (data) {
        s->STORE = true;
        p->msg.head.len += on_get_state((char*)s->data, sizeof(s->data), data);
      } else {
        s->FETCH = true;
      }
    } break;
    default:
      break;
  }
  return p;
}

/**
 * @brief Get the checksum for the given data
 *
 * @param data Pointer to the data
 * @param len Length of the data
 * @return uint8_t Checksum value
 */
uint8_t e12::get_checksum(const char* data, uint8_t len) {
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < len; i++) {
    checksum ^= data[i];
  }
  return checksum;
}

/**
 * @brief Get the message from the on-wire packet
 *
 * @param data Pointer to the data packet to store the message
 * @return true If the message was successfully retrieved
 * @return false If there was an error retrieving the message
 */
bool e12::get_message(e12_packet_t* data) {
  e12_onwire_t* pkt = get_decode_buffer();
  if (!memcpy(data, &pkt->data, sizeof(e12_packet_t))) {
#if ESP32_E12_SPEC
    ESP_LOGE(TAG, "Failed to copy message data");
#endif
    return false;
  }
  return true;
}

/**
 * @brief Flush the given buffer
 *
 * @param buf Pointer to the buffer to flush
 */
void e12::flush_buffer(e12_onwire_t* buf) {
  if (buf) {
    memset(buf, 0, sizeof(e12_onwire_t));
#if ESP32_E12_SPEC
    ESP_LOGI(TAG, "Buffer flushed");
#endif
  }
}

/**
 * @brief Get a response packet for the given request packet
 *
 * @param p Pointer to the request packet
 * @return e12_packet_t* Pointer to the response packet
 */
e12_packet_t* e12::get_response(e12_packet_t* p) {
  if (!p->msg.head.RESP_EXPECTED) return NULL;

  e12_packet_t* resp = e12_get_packet();
  if (!resp) {
#if ESP32_E12_SPEC
    ESP_LOGE(TAG, "Failed to get response packet");
#endif
    return NULL;
  }
  resp->msg.head.seq = p->msg.head.seq;
  resp->msg.head.IS_RESPONSE = true;
  resp->msg.head.cmd = p->msg.head.cmd;
  switch (p->msg.head.cmd) {
    case e12_cmd_t::CMD_PING: {
      resp->msg.head.len = strlen(STR_PONG) + 1;
      strcpy(resp->msg.data, STR_PONG);
    } break;
    case e12_cmd_t::CMD_TIME: {
      resp->msg_time.ms = get_time_ms();
      resp->msg.head.len = sizeof(uint32_t);
    } break;
    case e12_cmd_t::CMD_CONFIG: {
      resp->msg.head.len = sizeof(_dev_ptr->config);
      if (!memcpy(resp->msg.data, &_dev_ptr->config, resp->msg.head.len)) {
#if ESP32_E12_SPEC
        ESP_LOGE(TAG, "Failed to copy config data");
#endif
        return NULL;
      }
    } break;
    case e12_cmd_t::CMD_STATE: {
      e12_data_t* state = (e12_data_t*)p->msg.data;
      if (state->FETCH) {
        if (!memcpy(resp->msg.data, &_dev_ptr->state, sizeof(e12_data_t))) {
#if ESP32_E12_SPEC
          ESP_LOGE(TAG, "Failed to copy state data");
#endif
          return NULL;
        }
        state = (e12_data_t*)resp->msg.data;
        state->STORE = true;
        resp->msg.head.len = sizeof(e12_data_t);
      }
    } break;
    default:
      break;
  }
  return resp;
}

/**
 * @brief Handle the received packet
 *
 * @param p Pointer to the received packet
 * @return int Status code
 */
int e12::on_receive(e12_packet_t* p) {
  switch (p->msg.head.cmd) {
    case e12_cmd_t::CMD_CONFIG: {
      e12_data_t* config = (e12_data_t*)p->msg.data;
      if (config->IS_JSON) {
        _status.CONFIGURED =
            on_config((const char*)config->data, sizeof(config->data));
        return 0;
      }
    } break;
    case e12_cmd_t::CMD_STATE: {
      e12_data_t* state = (e12_data_t*)p->msg.data;
      if (state->IS_JSON && state->STORE) {
        on_restore_state((const char*)state->data, sizeof(state->data));
        return 0;
      }
    } break;
  }
  return 0;
}

/**
 * @brief Decode the given data into a packet
 *
 * @param pkt Pointer to the on-wire packet
 * @param data Data byte to decode
 * @return e12_packet_t* Pointer to the decoded packet
 */
e12_packet_t* e12::decode(e12_onwire_t* pkt, uint8_t data) {
#if ESP32_E12_SPEC
  ESP_LOGI(TAG, "e12::decode(%x)", data);
#endif
  pkt->buf[pkt->recv_len++] = data;
  if (pkt->recv_len > sizeof(e12_onwire_head_t)) {
    if (pkt->recv_len == pkt->head.len) {
      // we received the full packet. We can do the checksum
      // validation
      pkt->recv_len = 0;
      uint8_t checksum =
          get_checksum((const char*)&pkt->data, sizeof(e12_packet_t));
      if (checksum != pkt->checksum) {
#if ESP32_E12_SPEC
        ESP_LOGE(
            TAG,
            "e12::decode(CHECKSUM FAILED, expected = %02x, received = %02x)",
            pkt->checksum, checksum);
#endif
        return NULL;
      }
      return &pkt->data;
    }
  } else if (pkt->recv_len == E12_MAGIC_MARKER_LEN) {
    // we should check if we got the magic headers
    if (pkt->head.magic[0] != E12_MAGIC_MARKER_1 &&
        pkt->head.magic[1] != E12_MAGIC_MARKER_2) {
      pkt->recv_len = 0;
      return NULL;
    }
  }
  return NULL;
}

/**
 * @brief Get a new packet for encoding
 *
 * @return e12_packet_t* Pointer to the new packet
 */
e12_packet_t* e12::e12_get_packet() {
  e12_onwire_t* pkt = get_encode_buffer();
  memset(pkt, 0, sizeof(e12_onwire_t));
  pkt->data.msg.head.seq = ++_seq;
  return &pkt->data;
}
