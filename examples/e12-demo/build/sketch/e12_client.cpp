#line 1 "D:\\work\\e12-io\\e12-firmware-esp32\\lib\\e12-protocol\\examples\\e12-demo\\e12_client.cpp"

#include <ArduinoJson.h>
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

#define WAKEUP_INTR 0
#define WAKEUP_INTR_PIN 22

// NOTE: here can be used to debug. In most cases not needed
int e12_client::send(e12_packet_t* buf) {
  // TODO: here you can write some debug/forensic on
  // buffer aka wireshark
  e12_arduino::send(buf);
  return 0;
}

static void wakeme_up_from_sleep() {
  Serial.println("**********WHO WOKE ME UP ?***********");
}

int e12_client::wakeup_e12() {
  Serial.println("**********WAKE UP e12 ***********");
  // Briefly pulse the pin HIGH then LOW

  pinMode(WAKEUP_INTR_PIN, OUTPUT);
  digitalWrite(WAKEUP_INTR_PIN, HIGH);
  delay(10);
  digitalWrite(WAKEUP_INTR_PIN, LOW);
  return 0;
};

int e12_client::sleep(uint32_t ms, void* data) {
  Serial.println("e12_client::sleep");
  int err = e12_arduino::sleep(ms, data);
  if (err) return err;

#ifdef __AVR__
  // do your board specific power management
  // e.g sleep till you get woken up by wakeup pin interrupt
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  // use interrupt 0 (pin 2)
  attachInterrupt(WAKEUP_INTR, wakeme_up_from_sleep, LOW);
  Serial.print("Arduino - I SLEEP : ");
  Serial.println(get_time_ms());

  sleep_mode();

  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  sleep_disable();
  detachInterrupt(WAKEUP_INTR);
  power_all_enable();  // Re-enable the peripherals.
  Serial.print("Arduino - wakeup after sleep : ");
  Serial.println(get_time_ms());

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
  e12_arduino::on_receive(p);
  Serial.print("Received : ");
  switch (p->msg.head.cmd) {
    case e12_cmd_t::CMD_PING: {
      Serial.println((char*)p->msg.data);
    } break;
    case e12_cmd_t::CMD_TIME: {
      uint32_t ms = p->msg_time.ms;
      Serial.print("Received Time ms: ");
      Serial.println(ms);
    } break;
    case e12_cmd_t::CMD_CONFIG: {
    } break;
    case e12_cmd_t::CMD_STATE: {
    } break;
    case e12_cmd_t::CMD_WAKE_ME_UP: {
      Serial.println("OK");
    } break;
  }

  return 0;
}
