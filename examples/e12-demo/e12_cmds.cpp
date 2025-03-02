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
static bool process = false;

int process_command(const char* cmd) {
  if (strcmp(cmd, "help") == 0) {
    print_help();
    return E12_DEMO_HELP;
  }
  switch (atoi(cmd)) {
    case 1:
      return E12_SEND_PING;
    case 2:
      return E12_SEND_STATE;
    case 3:
      return E12_FETCH_CONFIG;
    case 4:
      return E12_SEND_AUTH;
    case 5:
      return E12_NODE_ACTIVATE_AP;
    case 6:
      return E12_SEND_LOG;
    case 7:
      return E12_NODE_SEND_STATUS;
    case 8:
      return E12_NODE_TRANSMIT;
    case 9:
      return E12_WAKE_ME_UP;
    case 10:
      return E12_NODE_WAKEUP;
    case 11:
      return E12_NODE_INITIATE_OTA;
    case 12:
      return E12_NODE_BLINK;
    case 13:
      return E12_NODE_DISABLE_SLEEP;
    case 14:
      return E12_GET_TIME;
    case 15:
      return E12_SEND_WIFI_AUTH;
    default:
      return E12_CMD_NONE;
  }
}

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
  int e12_cmd = process_command(cmd);
  memset(cmd, 0, MAX_CMD_LEN);
  return e12_cmd;
}
