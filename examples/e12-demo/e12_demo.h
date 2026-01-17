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
  EVT_CTL = 0x04,
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

  // virtual function implemented
  int on_config(const char* s, int len);
  int on_get_state(char* s, int len, void* ctx);
  int on_restore_state(const char* s, int len);

  int on_ctl_read(uint8_t pin);
  bool on_ctl_write(uint8_t pin, uint32_t val);

  int log(uint8_t type, uint8_t status, uint32_t ts, void* data);

  // client device functions
  uint32_t blink();
  uint32_t read_temp(DallasTemperature* sensors);
  uint32_t demo();
};

#endif
