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

#ifndef H_E12_CLIENT
#define H_E12_CLIENT

#include <Wire.h>

#include "Arduino.h"
//
#include <e12_protocol.h>

#include "arduino/arduino_e12_protocol.h"

class e12_client : public e12_arduino {
 private:
 public:
  e12_client(uint32_t vid, uint32_t pid) : e12_arduino(vid, pid) {}

  ~e12_client() {}
  int wakeup_e12_node();
  int sleep(uint32_t ms, void* data);
  int send(e12_packet_t* buf, bool retry = true);
  int on_receive(e12_packet_t* p);
};

#endif
