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

#include <stdio.h>

#include "Arduino.h"
#include "e12_cmds.h"
#include "e12_demo.h"
#include "e12_variants.h"

#define E12_BUS_ADDRESS 0x30

#define E12HQ_VENDOR_ID 42
#define E12HQ_DEMO_PRODUCT_ID 42

e12_demo demo(E12HQ_VENDOR_ID, E12HQ_VENDOR_ID);

static uint8_t event_flag = 0x00;

#if ARDUINO_RASPBERRY_PI_PICO
struct repeating_timer timer_blink;
struct repeating_timer timer_temp;
bool timer_callback_blink(struct repeating_timer *t) {
  event_flag |= 0x01;
  return true;
}

bool timer_callback_temp(struct repeating_timer *t) {
  event_flag |= (0x01 << 1);
  return true;
}
#elif ARDUINO_SAMD_ZERO  //__SAMD21__
#include <fast_samd21_tc3.h>
#include <fast_samd21_tc4_tc5.h>

#define LED_PIN LED_BUILTIN
void TC3_Handler(void) {
  event_flag |= 0x01;
  TC3->COUNT16.INTFLAG.bit.MC0 = 1;  // clears the interrupt
}

void TC4_Handler(void) {
  event_flag |= (0x01 << 1);
  TC4->COUNT32.INTFLAG.bit.MC0 = 1;  // clears the interrupt
}

#endif

#include <DallasTemperature.h>
#include <OneWire.h>

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define E12_INTR_PIN 3

#define DEBUG 1
#if DEBUG
const byte ledPin = LED_BUILTIN;
#endif

volatile byte e12_read_msg = false;
void e12_intr_handler() {
#if DEBUG
  digitalWrite(ledPin, true);
#endif
  e12_read_msg = true;
}

static void e12_intr() {
  pinMode(E12_INTR_PIN, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(E12_INTR_PIN), e12_intr_handler, HIGH);
}

void setup() {
#ifdef ARDUINO_RASPBERRY_PI_PICO
  add_repeating_timer_ms(60000, timer_callback_blink, NULL, &timer_blink);
  add_repeating_timer_ms(120000, timer_callback_temp, NULL, &timer_temp);
#elif ARDUINO_SAMD_ZERO
  fast_samd21_tc3_configure(1000000);  // blink every 1 sec
  fast_samd21_tc4_tc5_configure(5000000);
#else
#warning "No timer code defined for this board"
#endif

  Serial.begin(115200);
  demo.begin(&Wire, E12_BUS_ADDRESS);
  sensors.begin();

  // enable interrupt handling
  // once triggered then read e12 message
  e12_intr();

  // here we activate the Wifi and ask e12 node to
  // always fetch its configaration from the server
  //  e12_node_properties_t p = {.REFRESH_CONFIG = true, .ACTIVATE_WIFI = true};
  e12_node_properties_t p = {0};
  p.REFRESH_CONFIG = true;
  p.ACTIVATE_WIFI = true;
  demo.set_node_properties(&p);
}

void loop() {
  uint32_t cmd = 0;
  if ((cmd = demo.demo()) >= 0) {
    switch (cmd) {
      // handle demo commands not part of standard handling
      // e.g library would not know/hard code the wifi auth
      // info
      case E12_SEND_WIFI_AUTH: {
        Serial.println("Executing: e12 wifi auth exchange");
        //        e12_auth_data_t auth = {.AUTH_WIFI = true};
        e12_auth_data_t auth = {0};
        auth.AUTH_WIFI = true;
        strcpy(auth.wifi.ssid, "WWZ-444703");
        strcpy(auth.wifi.pwd, "FNA7UQGAP7V7B3");
        demo.set_node_auth_credentials(&auth);
      } break;
      default:

        break;
    }
  }

  if (e12_read_msg) {
    demo.on_receive(demo.read());
#if DEBUG
    digitalWrite(ledPin, false);
#endif
    e12_read_msg = false;
  }

  uint8_t mask = 0x01;
  while (event_flag && mask != 0) {
    switch ((event_flag & mask)) {
      case EVT_BLINK: {
        // Serial.println("Executing: blink");
        demo.blink();
      } break;
      case EVT_TEMP: {
        // Serial.println("Executing: Read temperature");
        demo.read_temp(&sensors);
      } break;
    }
    event_flag &= ~mask;
    mask = mask << 1;
  }

  // here ideally in a real device you will
  // got to sleep waiting for events to happen
}
