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

#include <ArduinoJson.h>
#ifdef __AVR__
#include <avr/power.h>
#include <avr/sleep.h>
#endif

#include "Arduino.h"
#include "e12_cmds.h"
#include "e12_demo.h"

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
  Serial.print("Temp Sensor ( ");
  Serial.print(0);
  Serial.print(") : ");
  Serial.print(tempC);
  Serial.println("C");
  log((uint8_t)EVT_TEMP, (uint8_t)e12_evt_status_t::STATUS_DONE, get_time_ms(),
      (void*)&tempC);
  return 0;
}

int e12_demo::on_config(const char* s, int len) {
  // called with the JSON string
  // e.g {"v":"0.0.1","pid":48876,"vid":57005,"config":{"pin":4,"delay":1000}}

  Serial.println("**********ARDUINO GOT JSON CONFIG ***********");
  Serial.println(s);

  StaticJsonDocument<128> doc;
  deserializeJson(doc, s);

  const char* v = doc["v"];
  int vid = doc["vid"];
  int pid = doc["pid"];
  int pin = doc["config"]["pin"];
  uint32_t on_delay = doc["config"]["on_ms"];
  uint32_t off_delay = doc["config"]["off_ms"];

  // _pin = pin;
  _on_delay = on_delay;
  _off_delay = off_delay;
  return true;
}

int e12_demo::on_get_state(char* s, int len, void* ctx) {
  Serial.println("**********ARDUINO GOT GET STATE ***********");
  StaticJsonDocument<128> doc;
  doc["count"] = _count;
  doc["on"] = _on;
  int count = serializeJson(doc, s, len);
  Serial.println(s);
  return count;
}

int e12_demo::on_restore_state(const char* s, int len) {
  Serial.println("**********ARDUINO RESTORE STATE ***********");
  Serial.println(s);
  // here we expecting JSON whatever is state worth
  StaticJsonDocument<128> doc;
  deserializeJson(doc, s);
  _count = doc["count"];
  _on = doc["on"];
  return 0;
}

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
  send(get_request(e12_cmd_t::CMD_LOG, true, evt));
  evt->in_use = false;
  return 0;
};

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
      send(get_request(e12_cmd_t::CMD_PING, true, NULL));
    } break;
    case E12_SEND_STATE: {
      Serial.println("Executing: sending state to e12 node");
      send(get_request(e12_cmd_t::CMD_STATE));
    } break;
    case E12_FETCH_CONFIG: {
      Serial.println("Executing: Request config from e12 node");
      send(get_request(e12_cmd_t::CMD_CONFIG));
    } break;
    case E12_GET_TIME: {
      Serial.println("Executing: Requesting TIME from e12 node");
      send(get_request(e12_cmd_t::CMD_TIME));
    } break;
    case E12_SCHEDULE_WAKEUP: {
      Serial.println("Executing: WAKE ME UP in 10 sec");
      e12_wakeup_data_t wakeup = {0};
      wakeup.ms = 10000;
      send(get_request(e12_cmd_t::CMD_SCHEDULE_WAKEUP, true, (void*)&wakeup));
    } break;
    case E12_NODE_WAKEUP: {
      Serial.println("Executing: e12 node wakeup");
      wakeup_e12_node();
      return 0;
    } break;
    case E12_NODE_TRANSMIT: {
      Serial.println("Executing: e12 node TRANSMIT");
      e12_node_properties_t p = {0};
      p.TRANSMIT = true;
      send(get_request(e12_cmd_t::CMD_SET_NODE_PROPERTIES, true, (void*)&p));
    } break;
    case E12_NODE_DISABLE_SLEEP: {
      Serial.println("Executing: e12 node DISABLE SLEEP");
      e12_node_properties_t p = {0};
      p.DISABLE_SLEEP = true;
      send(get_request(e12_cmd_t::CMD_SET_NODE_PROPERTIES, true, (void*)&p));
    } break;
    case E12_NODE_BLINK: {
      Serial.println("Executing: e12 node blink");
      e12_debug_blink_t blink = {.on_ms = 1000, .off_ms = 2000, .count = 10};
      send(get_request(e12_cmd_t::CMD_DEBUG_BLINK, true, (void*)&blink));
    } break;
    case E12_NODE_ACTIVATE_AP: {
      Serial.println("Executing: activate captive portal on e12 node");
      e12_node_properties_t p = {0};
      p.ACTIVATE_WIFI_CAPTIVE_PORTAL = true;
      send(get_request(e12_cmd_t::CMD_SET_NODE_PROPERTIES, true, (void*)&p));
    } break;
    // once the client code should handle
    case E12_SEND_WIFI_AUTH: {
      return E12_SEND_WIFI_AUTH;
    } break;
    case E12_NODE_LOGMASK: {
      Serial.println("Executing: e12 logmask");
      e12_node_properties_t p = {0};
      p.LOGMASK = true;
      p.data = (uint64_t)0x01 << 26;  // dont log DEBUG_BLINK
      send(get_request(e12_cmd_t::CMD_SET_NODE_PROPERTIES, true, (void*)&p));
    } break;
    default: {
      return -1;
    }
  }
  return 0;
}
