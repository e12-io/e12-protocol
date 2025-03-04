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

#ifndef H_E12_CMDS
#define H_E12_CMDS

enum {
  E12_CMD_NONE,
  E12_DEMO_HELP,
  E12_SEND_PING,
  E12_SEND_STATE,
  E12_FETCH_CONFIG,
  E12_REFRESH_CONFIG,
  E12_SEND_AUTH,
  E12_SEND_WIFI_AUTH,
  E12_SCHEDULE_WAKEUP,
  E12_SEND_LOG,
  E12_GET_TIME,
  E12_NODE_ACTIVATE_AP,
  E12_NODE_SEND_STATUS,
  E12_NODE_TRANSMIT,
  E12_NODE_WAKEUP,
  E12_NODE_INITIATE_OTA,
  E12_NODE_DISABLE_SLEEP,
  E12_NODE_LOGMASK,
  E12_NODE_BLINK,
} e12_demo_cmd_t;

int get_cmd();

#endif