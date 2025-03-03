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

#ifndef H_ESP32_E12_SPEC
#define H_ESP32_E12_SPEC

#include <Wire.h>
#include <e12_protocol.h>
#include <stdint.h>

/**
 * @class e12_esp32_node
 * @brief This class represents an ESP32 node for the e12 protocol.
 *
 * The e12_esp32_node class provides methods for initializing the node,
 * handling communication, managing events, configuring the node, and
 * performing utility functions. It extends the base e12 class and
 * integrates with the TwoWire interface for I2C communication.
 */
class e12_esp32_node : public e12 {
 private:
  TwoWire* _bus;  ///< Pointer to the I2C bus interface

 public:
  /**
   * @brief Constructor for the e12_esp32_node class.
   * @param vid Vendor ID
   * @param pid Product ID
   */
  e12_esp32_node(uint32_t vid, uint32_t pid);

  /**
   * @brief Destructor for the e12_esp32_node class.
   */
  ~e12_esp32_node();

  // Initialization

  /**
   * @brief Initializes the node with the given bus and address.
   * @param bus Pointer to the I2C bus
   * @param e12_addr Address of the e12 node
   * @return 0 on success, non-zero on failure
   */
  virtual int begin(void* bus, uint8_t e12_addr = 0);

  // Time

  /**
   * @brief Gets the current time in milliseconds.
   * @return Current time in milliseconds
   */
  virtual uint32_t get_time_ms();

  // Communication

  /**
   * @brief Sends a packet to the e12 node.
   * @param buf Pointer to the packet buffer
   * @return 0 on success, non-zero on failure
   */
  virtual int send(e12_packet_t* buf);

  /**
   * @brief Reads a packet from the e12 node.
   * @return Pointer to the received packet, or NULL on failure
   */
  virtual e12_packet_t* read();

  // Event handling

  /**
   * @brief Gets the log event.
   * @return Pointer to the log event
   */
  virtual e12_log_evt_t* get_log_evt();

  /**
   * @brief Logs an event.
   * @param type Event type
   * @param status Event status
   * @param ts Timestamp
   * @param data Pointer to additional data
   * @return 0 on success, non-zero on failure
   */
  virtual int log(uint8_t type, uint8_t status, uint32_t ts, void* data);

  // Configuration

  /**
   * @brief Configures the node with the given settings.
   * @param s Pointer to the JSON configuration string
   * @param len Length of the configuration string
   * @return 0 on success, non-zero on failure
   */
  virtual int on_config(const char* s, int len);

  /**
   * @brief Gets the current state of the node.
   * @param s Pointer to the JSON state string buffer
   * @param len Length of the state string buffer
   * @param ctx Pointer to the context
   * @return 0 on success, non-zero on failure
   */
  virtual int on_get_state(char* s, int len, void* ctx);

  /**
   * @brief Restores the state of the node from the given string.
   * @param s Pointer to the JSON state string
   * @param len Length of the state string
   * @return 0 on success, non-zero on failure
   */
  virtual int on_restore_state(const char* s, int len);

  /**
   * @brief Sets the authentication credentials for the node.
   * @param auth Pointer to the authentication data
   * @return 0 on success, non-zero on failure
   */
  virtual int set_node_auth_credentials(e12_auth_data_t* auth);

  // Power management

  /**
   * @brief Wakes up the node.
   * @return 0 on success, non-zero on failure
   */
  virtual int on_wakeup();

  /**
   * @brief Puts the node to sleep for the specified duration.
   * @param ms Sleep duration in milliseconds
   * @param data Pointer to additional data
   * @return 0 on success, non-zero on failure
   */
  virtual int sleep(uint32_t ms, void* data);

  // Utility

  /**
   * @brief Prints the contents of the buffer.
   * @param buf Pointer to the buffer
   * @return 0 on success, non-zero on failure
   */
  virtual int print_buffer(e12_onwire_t* buf);
};

#endif