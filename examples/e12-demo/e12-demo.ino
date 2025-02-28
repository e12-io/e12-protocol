#include <stdio.h>

#include "Arduino.h"
#include "e12_cmds.h"
#include "e12_demo.h"
#include "pico/stdlib.h"

#define E12_BUS_ADDRESS 0x30

#define E12HQ_VENDOR_ID 42
#define E12HQ_DEMO_PRODUCT_ID 42

e12_demo demo(E12HQ_VENDOR_ID, E12HQ_VENDOR_ID);

struct repeating_timer timer_blink;
struct repeating_timer timer_temp;

static uint8_t event_flag = 0x00;
bool timer_callback_blink(struct repeating_timer *t) {
  event_flag |= 0x01;
  return true;
}

bool timer_callback_temp(struct repeating_timer *t) {
  event_flag |= (0x01 << 1);
  return true;
}

#include <DallasTemperature.h>
#include <OneWire.h>
#define ONE_WIRE_BUS 15

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define E12_INTR_PIN 2

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
  Serial.begin(115200);
  demo.begin(&Wire, E12_BUS_ADDRESS);
  sensors.begin();

  add_repeating_timer_ms(5000, timer_callback_blink, NULL, &timer_blink);
  add_repeating_timer_ms(10000, timer_callback_temp, NULL, &timer_temp);

  // here we activate the Wifi and ask e12 node to
  // always fetch its configaration from the server
  e12_node_properties_t p = {.REFRESH_CONFIG = true, .ACTIVATE_WIFI = true};
  demo.set_node_properties(&p);

  // enable interrupt handling
  // once triggered then read e12 message
  e12_intr();
}

#define E12_WAKEUP_WAIT 1000
void loop() {
  uint32_t cmd = 0;
  if ((cmd = demo.demo()) >= 0) {
    switch (cmd) {
      // handle demo commands not part of standard handling
      // e.g library would not know/hard code the wifi auth
      // info
      case E12_SEND_WIFI_AUTH: {
        Serial.println("Executing: e12 wifi auth exchange");
        e12_auth_data_t auth = {.AUTH_WIFI = true};
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
        demo.blink();
      } break;
      case EVT_TEMP: {
        demo.read_temp(&sensors);
      } break;
    }
    event_flag &= ~mask;
    mask = mask << 1;
  }

  // here ideally in a real device you will
  // got to sleep waiting for events to happen
}
