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

#ifdef __AVR__
#include <avr/power.h>
#include <avr/sleep.h>
#endif

#include "Arduino.h"
#include "e12_cmds.h"
#include "e12_demo.h"
#include "e12_variants.h"

static long json_get_val(const char* s, const char* key) {
  const char* ptr = strstr(s, key);
  if (!ptr) return 0;
  ptr += strlen(key);
  while (*ptr && (*ptr == ':' || *ptr == ' ' || *ptr == '\"')) {
    ptr++;
  }

  return atol(ptr);
}

e12_demo::e12_demo(uint32_t vid, uint32_t pid) : e12_client(vid, pid) {
  _on = false;
  _pin = LED_BUILTIN;
  _on_delay = 5000;
  _off_delay = 60000;
  _count = 0;
  _temp = 0.0f;

  pinMode(LED_BUILTIN, OUTPUT);
}

uint32_t e12_demo::blink() {
  _count++;
  _on = !_on;
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, _on);
  log((uint8_t)EVT_BLINK, (uint8_t)e12_evt_status_t::STATUS_DONE, get_time_ms(),
      (void*)&_on);
  return (_on) ? _on_delay : _off_delay;
}

uint32_t e12_demo::read_temp(DallasTemperature* sensors) {
  sensors->requestTemperatures();
  float tempC = sensors->getTempCByIndex(0);
  E12_PRINT_F("Temp Sensor (%d) : %dC", 0, (int)tempC);
  log((uint8_t)EVT_TEMP, (uint8_t)e12_evt_status_t::STATUS_DONE, get_time_ms(),
      (void*)&tempC);
  return 0;
}


int e12_demo::on_config(const char* s, int len) {
  // s = {"pin":4,"on_ms":1000, "off_ms":2000}
  E12_PRINTLN("**********ARDUINO GOT JSON CONFIG ***********");
  E12_PRINTLN(s);

  _on_delay = json_get_val(s, "\"on_ms\"");
  _off_delay = (uint8_t)json_get_val(s, "\"off_ms\"");

  return true;
}

int e12_demo::on_get_state(char* s, int len, void* ctx) {
  strcpy_P(s, PSTR("{\"count\":"));

  // Write the number directly into 's' starting at the current end
  // 's + strlen(s)' points to the null terminator
  ultoa(_count, s + strlen(s), 10);

  strcat_P(s, PSTR(",\"on\":"));
  strcat(s, _on ? "1" : "0");
  strcat(s, "}");

  return strlen(s);
}

int e12_demo::on_restore_state(const char* s, int len) {
  E12_PRINTLN("**********ARDUINO RESTORE STATE ***********");
  E12_PRINTLN(s);

  _count = json_get_val(s, "\"count\"");
  _on = (uint8_t)json_get_val(s, "\"on\"");

  return 0;
}


// publish log events to e12-node
int e12_demo::log(uint8_t type, uint8_t status, uint32_t ts, void* data) {
  e12_log_evt_t* evt = e12_arduino::get_log_evt();
  if (!evt) return -1;

  evt->type = type;
  evt->status = status;
  evt->ts = ts;
  switch (type) {
    case (uint8_t)EVT_DEMO:
    case (uint8_t)EVT_BLINK: {
      evt->i_data = *((uint8_t*)data);
      evt->i = true;
    } break;
    case (uint8_t)EVT_TEMP: {
      evt->f_data = *((float*)data);
      evt->f = true;
    } break;
  }
  send(get_request(e12_cmd_t::CMD_LOG, true, evt), true);
  evt->in_use = false;
  return 0;
};

// demo loop to demonstrate the e12-protocol capabilities
uint32_t e12_demo::demo() {
  int cmd = get_cmd();
  if (cmd > E12_CMD_NONE) {
    log((uint8_t)EVT_DEMO, (uint8_t)e12_evt_status_t::STATUS_DONE,
        get_time_ms(), (void*)&cmd);
  } else {
    return E12_CMD_NONE;
  }

  switch (cmd) {
    case E12_SEND_PING: {
      send(get_request(e12_cmd_t::CMD_PING, true, NULL), true);
    } break;
    case E12_SEND_STATE: {
      E12_PRINTLN("Executing: sending state to e12 node");
      send(get_request(e12_cmd_t::CMD_STATE, true, (void*)true), true);
    } break;
    case E12_FETCH_CONFIG: {
      E12_PRINTLN("Executing: Request config from e12 node");
      send(get_request(e12_cmd_t::CMD_CONFIG), true);
    } break;
    case E12_SEND_INFO: {
      E12_PRINTLN("Executing: Publishing vmcu info");
      send(get_request(e12_cmd_t::CMD_INFO), true);
    } break;
    case E12_GET_TIME: {
      E12_PRINTLN("Executing: Requesting TIME from e12 node");
      send(get_request(e12_cmd_t::CMD_TIME), true);
    } break;
    case E12_SCHEDULE_WAKEUP: {
      E12_PRINTLN("Executing: WAKE ME UP in 10 sec");
      e12_wakeup_data_t wakeup = {0};
      wakeup.ms = 10000;
      send(get_request(e12_cmd_t::CMD_SCHEDULE_WAKEUP, true, (void*)&wakeup),
           true);
    } break;
    case E12_NODE_WAKEUP: {
      E12_PRINTLN("Executing: e12 node wakeup");
      wakeup_e12_node();
      return 0;
    } break;
    case E12_NODE_TRANSMIT: {
      E12_PRINTLN("Executing: e12 node TRANSMIT");
      e12_node_properties_t p = {0};
      p.TRANSMIT = true;
      send(get_request(e12_cmd_t::CMD_SET_NODE_PROPERTIES, true, (void*)&p),
           true);
    } break;
    case E12_NODE_DISABLE_SLEEP: {
      E12_PRINTLN("Executing: e12 node DISABLE SLEEP");
      e12_node_properties_t p = {0};
      p.DISABLE_SLEEP = true;
      send(get_request(e12_cmd_t::CMD_SET_NODE_PROPERTIES, true, (void*)&p),
           true);
    } break;
    case E12_NODE_BLINK: {
      E12_PRINTLN("Executing: e12 node blink");
      e12_debug_blink_t blink = {.on_ms = 1000, .off_ms = 2000, .count = 10};
      send(get_request(e12_cmd_t::CMD_DEBUG_BLINK, true, (void*)&blink), true);
    } break;
    case E12_NODE_ACTIVATE_AP: {
      E12_PRINTLN("Executing: activate captive portal on e12 node");
      e12_node_properties_t p = {0};
      p.ACTIVATE_WIFI_CAPTIVE_PORTAL = true;
      send(get_request(e12_cmd_t::CMD_SET_NODE_PROPERTIES, true, (void*)&p),
           true);
    } break;
    // once the client code should handle
    case E12_SEND_WIFI_AUTH: {
      return E12_SEND_WIFI_AUTH;
    } break;
    case E12_NODE_LOGMASK: {
      E12_PRINTLN("Executing: e12 logmask");
      e12_node_properties_t p = {0};
      p.LOGMASK = true;
      p.data = (uint64_t)0x01 << 26;  // dont log DEBUG_BLINK
      send(get_request(e12_cmd_t::CMD_SET_NODE_PROPERTIES, true, (void*)&p),
           true);
    } break;
    case E12_NODE_INITIATE_OTA: {
      send(get_request(e12_cmd_t::CMD_OTA), true);
    } break;
    case E12_NODE_INITIATE_VMCU_OTA: {
      E12_PRINTLN("Executing: VMCU OTA ... disconnect Serial");
      delay(5000);
      send(get_request(e12_cmd_t::CMD_VMCU_OTA), true);
    } break;
    default: {
      return -1;
    }
  }
  return 0;
}
