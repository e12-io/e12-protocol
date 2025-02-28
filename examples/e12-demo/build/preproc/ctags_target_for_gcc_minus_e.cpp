# 1 "D:\\work\\e12-io\\e12-firmware-esp32\\lib\\e12-protocol\\examples\\e12-demo\\e12-demo.ino"
# 2 "D:\\work\\e12-io\\e12-firmware-esp32\\lib\\e12-protocol\\examples\\e12-demo\\e12-demo.ino" 2

# 4 "D:\\work\\e12-io\\e12-firmware-esp32\\lib\\e12-protocol\\examples\\e12-demo\\e12-demo.ino" 2
# 5 "D:\\work\\e12-io\\e12-firmware-esp32\\lib\\e12-protocol\\examples\\e12-demo\\e12-demo.ino" 2
# 6 "D:\\work\\e12-io\\e12-firmware-esp32\\lib\\e12-protocol\\examples\\e12-demo\\e12-demo.ino" 2
# 7 "D:\\work\\e12-io\\e12-firmware-esp32\\lib\\e12-protocol\\examples\\e12-demo\\e12-demo.ino" 2






e12_demo demo(42, 42);

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





OneWire oneWire(15);
DallasTemperature sensors(&oneWire);





const byte ledPin = (25u);


volatile byte e12_read_msg = false;
void e12_intr_handler() {

  digitalWrite(ledPin, true);

  e12_read_msg = true;
}

static void e12_intr() {
  pinMode(2, INPUT_PULLDOWN);
  attachInterrupt((2), e12_intr_handler, HIGH);
}

void setup() {
  Serial.begin(115200);
  demo.begin(&Wire, 0x30);
  sensors.begin();

  add_repeating_timer_ms(5000, timer_callback_blink, 
# 61 "D:\\work\\e12-io\\e12-firmware-esp32\\lib\\e12-protocol\\examples\\e12-demo\\e12-demo.ino" 3 4
                                                    __null
# 61 "D:\\work\\e12-io\\e12-firmware-esp32\\lib\\e12-protocol\\examples\\e12-demo\\e12-demo.ino"
                                                        , &timer_blink);
  add_repeating_timer_ms(10000, timer_callback_temp, 
# 62 "D:\\work\\e12-io\\e12-firmware-esp32\\lib\\e12-protocol\\examples\\e12-demo\\e12-demo.ino" 3 4
                                                    __null
# 62 "D:\\work\\e12-io\\e12-firmware-esp32\\lib\\e12-protocol\\examples\\e12-demo\\e12-demo.ino"
                                                        , &timer_temp);

  // here we activate the Wifi and ask e12 node to
  // always fetch its configaration from the server
  e12_node_properties_t p = {.REFRESH_CONFIG = true, .ACTIVATE_WIFI = true};
  demo.set_node_properties(&p);

  // enable interrupt handling
  // once triggered then read e12 message
  e12_intr();
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

    digitalWrite(ledPin, false);

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
