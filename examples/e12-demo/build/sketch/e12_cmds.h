#line 1 "D:\\work\\e12-io\\e12-firmware-esp32\\lib\\e12-protocol\\examples\\e12-demo\\e12_cmds.h"
#ifndef H_E12_CMDS
#define H_E12_CMDS

enum {
  E12_CMD_NONE,
  E12_DEMO_HELP,
  E12_SEND_PING,
  E12_SEND_STATE,
  E12_FETCH_CONFIG,
  E12_SEND_AUTH,
  E12_SEND_WIFI_AUTH,
  E12_WAKE_ME_UP,
  E12_SEND_LOG,
  E12_GET_TIME,
  E12_NODE_ACTIVATE_AP,
  E12_NODE_SEND_STATUS,
  E12_NODE_TRANSMIT,
  E12_NODE_WAKEUP,
  E12_NODE_INITIATE_OTA,
  E12_NODE_DISABLE_SLEEP,
  E12_NODE_BLINK,
} e12_demo_cmd_t;

int get_cmd();

#endif