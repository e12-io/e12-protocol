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

#include "e12_cmds.h"

/*
IMPORTANT NOTE: PLEASE READ
we need i2c buffer length to be 128. Arduino stock Wire
library has buffer of 32. To change that you need to make changes
to Wire library: Wire.h and twi.h

#define BUFFER_LENGTH 128
#define TWI_BUFFER_LENGTH 128
*/

#include "Arduino.h"
#include "e12_client.h"
#include "e12_variants.h"

#define DEBUG 1
#define WAKEUP_INTR 0

int e12_client::send(e12_packet_t* buf, bool retry) {
  // TODO: here you can write some debug/forensic on
  // buffer ready to be sent aka wireshark
  e12_arduino::send(buf, retry);
  return 0;
}

static void wakeme_up_from_sleep() {}

int e12_client::wakeup_e12_node() {
  // Briefly pulse the pin HIGH then LOW
  pinMode(WAKEUP_INTR_PIN, OUTPUT);
  digitalWrite(WAKEUP_INTR_PIN, HIGH);
  delay(5);  // 1ms
  digitalWrite(WAKEUP_INTR_PIN, LOW);
  return 0;
};

int e12_client::sleep(uint32_t ms, void* data) {
  E12_PRINTLN(__func__);
  int err = e12_arduino::sleep(ms, data);
  if (err) return err;

#ifdef __AVR__
  // do your board specific power management
  // e.g sleep till you get woken up by wakeup pin interrupt
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  // use interrupt 0 (pin 2)
  attachInterrupt(WAKEUP_INTR, wakeme_up_from_sleep, LOW);

  E12_PRINT_F("Arduino - I SLEEP : (%d)", get_time_ms());

  sleep_mode();

  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  sleep_disable();
  detachInterrupt(WAKEUP_INTR);
  power_all_enable();  // Re-enable the peripherals.

  E12_PRINT_F("Arduino - woke up after : (%d)", get_time_ms());

  on_wakeup();
#else
#warning "Please implement platform dependent sleep function"
#if 0
#include <hardware/rosc.h>
#include <pico/sleep.h>
#include <pico/stdlib.h>

sleep_run_from_xosc();
sleep_goto_dormant_until_pin(15, true, true);  // Pin, Edge, HIGH
rosc_write(&rosc_hw->ctrl, ROSC_CTRL_ENABLE_BITS);
clocks_init();
set_sys_clock_khz(133000, true);

#endif
#endif
  return 0;
}

int e12_client::on_receive(e12_packet_t* p) {
  if (!p) return -1;

  e12_arduino::on_receive(p);

#if 0
  if (p->msg.head.IS_RESPONSE) {
    E12_PRINT_F("Received Response for cmd : (%d)", (int)p->msg.head.cmd);
  } else {
    E12_PRINT_F("Received Request cmd : (%d)", (int)p->msg.head.cmd);
  }
#endif

  switch (p->msg.head.cmd) {
    case e12_cmd_t::CMD_PING: {
      E12_PRINTLN((char*)p->msg.data);
    } break;
    case e12_cmd_t::CMD_TIME: {
      uint32_t ms = p->msg_time.ms;
      E12_PRINT_F("Received Time ms : (%u)", ms);
    } break;
    case e12_cmd_t::CMD_CONFIG: {
      E12_PRINTLN("Recieved CONFIG");
    } break;
    case e12_cmd_t::CMD_STATE: {
      E12_PRINTLN("Recieved STATE");
    } break;
    case e12_cmd_t::CMD_SCHEDULE_WAKEUP: {
      E12_PRINTLN("OK");
    } break;
    case e12_cmd_t::CMD_NODE_SLEEP: {
      E12_PRINT_F("CMD_NODE_SLEEP: (%u)", p->msg_sleep.ms);
    } break;
    case e12_cmd_t::CMD_PIN_CTL: {
      E12_PRINT_F("CMD_PIN_CTL: (%d:%d:%d)", p->msg_ctl.op, p->msg_ctl.pin,
                  p->msg_ctl.value);
    } break;
  }

  return 0;
}
