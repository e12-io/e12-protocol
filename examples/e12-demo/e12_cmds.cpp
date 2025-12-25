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
#include "e12_variants.h"

void print_help() {
  Serial.println("Welcome to e12 demo ....");
  Serial.println("1: Print this help message. Enter option number to trigger action !");
  Serial.println("2 : Send PING to e12 node");
  Serial.println("3 : Send STATE to e12 node");
  Serial.println("4 : Send mcu info");
  Serial.println("5 : Fetch my CONFIG from e12 node");
  Serial.println("6 : Refresh CONFIG from e12 server");
  Serial.println("7 : Send AUTH data to e12 node");
  Serial.println("8 : send WiFi Auth to e12 node");
  Serial.println("9 : WAKE ME UP after 10 sec");
  Serial.println("10 : Send a LOG event to e12 node");
  Serial.println("11 : get Time from e12 node");
  Serial.println("12 : Instruct e12 node to ACTIVATE AP");
  Serial.println("13 : Get e12 node STATUS");
  Serial.println("14 : Instruct e12 to TRANSMIT - NOW");
  Serial.println("15 : Send WAKEUP instruction to e12 node");
  Serial.println("16 : Ask e12 node to initiate OTA");
  Serial.println("17 : Initiate VMCU Flashing");
  Serial.println("18 : e12 node please DISABLE SLEEP");
  Serial.println("19 : set logmask for e12 node");
  Serial.println("20 : e12 node please BLINK");
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

  int command = atoi(cmd);
  if (command >= E12_DEMO_HELP && command <= E12_NODE_BLINK) {
    return command;
  }

  return E12_CMD_NONE;
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
