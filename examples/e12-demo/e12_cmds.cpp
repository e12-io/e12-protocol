#include "e12_cmds.h"

#include "Arduino.h"

void print_help() {
  Serial.println("Welcome to e12 demo ....");
  Serial.println("Enter option number to trigger action !");
  Serial.println("1 : Send PING to e12 node");
  Serial.println("2 : Send STATE to e12 node");
  Serial.println("3 : Fetch my CONFIG from e12 node");
  Serial.println("4 : Send AUTH data to e12 node");
  Serial.println("5 : Instruct e12 node to ACTIVATE AP");
  Serial.println("6 : Send a LOG event to e12 node");
  Serial.println("7 : Get e12 node STATUS");
  Serial.println("8 : Instruct e12 to TRANSMIT - NOW");
  Serial.println("9 : WAKE ME UP after 10 sec");
  Serial.println("10 : Send WAKEUP instruction to e12 node");
  Serial.println("11 : Ask e12 node to initiate OTA");
  Serial.println("12 : e12 node please BLINK");
  Serial.println("13 : e12 node please DISABLE SLEEP");
  Serial.println("14 : get Time from e12 node");
  Serial.println("15 : send WiFi Auth to e12 node");
}

#define MAX_CMD_LEN 32
static char cmd[MAX_CMD_LEN] = {0};
static int i = 0;
static int process = false;
int get_cmd() {
  while (Serial.available()) {
    char c = Serial.read();
    cmd[i] = c;
    if (c == '\n') {
      cmd[i] = 0;
      process = true;
      break;
    }
    i++;
    if (i >= MAX_CMD_LEN) {
      i = 0;
      memset(cmd, 0, MAX_CMD_LEN);
    }
  }

  if (!process) return E12_CMD_NONE;
  process = false;
  i = 0;
  int e12_cmd = E12_CMD_NONE;
  if (strcmp(cmd, "help") == 0) {
    print_help();
    e12_cmd = E12_DEMO_HELP;
  } else if (strcmp(cmd, "1") == 0) {
    e12_cmd = E12_SEND_PING;  // done
  } else if (strcmp(cmd, "2") == 0) {
    e12_cmd = E12_SEND_STATE;  // done
  } else if (strcmp(cmd, "3") == 0) {
    e12_cmd = E12_FETCH_CONFIG;  // done
  } else if (strcmp(cmd, "4") == 0) {
    e12_cmd = E12_SEND_AUTH;
  } else if (strcmp(cmd, "5") == 0) {
    e12_cmd = E12_NODE_ACTIVATE_AP;
  } else if (strcmp(cmd, "6") == 0) {
    e12_cmd = E12_SEND_LOG;
  } else if (strcmp(cmd, "7") == 0) {
    e12_cmd = E12_NODE_SEND_STATUS;
  } else if (strcmp(cmd, "8") == 0) {
    e12_cmd = E12_NODE_TRANSMIT;  // done
  } else if (strcmp(cmd, "9") == 0) {
    e12_cmd = E12_WAKE_ME_UP;  // done
  } else if (strcmp(cmd, "10") == 0) {
    e12_cmd = E12_NODE_WAKEUP;  // done
  } else if (strcmp(cmd, "11") == 0) {
    e12_cmd = E12_NODE_INITIATE_OTA;
  } else if (strcmp(cmd, "12") == 0) {
    e12_cmd = E12_NODE_BLINK;  // done
  } else if (strcmp(cmd, "13") == 0) {
    e12_cmd = E12_NODE_DISABLE_SLEEP;
  } else if (strcmp(cmd, "14") == 0) {
    e12_cmd = E12_GET_TIME;
  } else if (strcmp(cmd, "15") == 0) {
    e12_cmd = E12_SEND_WIFI_AUTH;
  } else {
  }

  memset(cmd, 0, MAX_CMD_LEN);
  return e12_cmd;
}
