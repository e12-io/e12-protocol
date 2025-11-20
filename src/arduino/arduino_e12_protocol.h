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

#ifndef ARDUINO_E12_SPEC_H
#define ARDUINO_E12_SPEC_H

#include <Wire.h>
#include <e12_protocol.h>
#include <stdint.h>

using namespace arduino;

/**
 * @brief Structure to hold event data.
 */
typedef struct e12_event {
  void* data;
} e12_event_t;

/**
 * @brief Class to handle e12 protocol on Arduino.
 */
class e12_arduino : public e12 {
 private:
  uint8_t _e12_addr;      ///< e12 device address
#ifdef ARDUINO_SAMD_ZERO  //__SAMD21__
#warning "SAMD21 detected"
  TwoWire* _bus;  ///< I2C bus
#else
  Wire* _bus;  ///< I2C bus
#endif
  uint32_t _evt_count;                      ///< Event count
  e12_log_evt_t _log[E12_MAX_LOG_BUFFERS];  ///< Log buffer

 public:
  /**
   * @brief Constructor for e12_arduino.
   * @param vid Vendor ID
   * @param pid Product ID
   */
  e12_arduino(uint32_t vid, uint32_t pid);

  /**
   * @brief Destructor for e12_arduino.
   */
  ~e12_arduino();

  /**
   * @brief Initialize the e12 device.
   * @param bus I2C bus
   * @param e12_addr e12 device address
   * @return int Status of initialization
   */
  virtual int begin(void* bus, uint8_t e12_addr = 0);

  /**
   * @brief Run the e12 protocol.
   */
  void e12_run();

  /**
   * @brief Close the e12 device.
   * @return int Status of close operation
   */
  int close();

  /**
   * @brief Get the current time in milliseconds.
   * @return uint32_t Current time in milliseconds
   */
  virtual uint32_t get_time_ms();

  /**
   * @brief Send a packet to the e12 device.
   * @param buf Packet buffer
   * @return int Status of send operation
   */
  virtual int send(e12_packet_t* buf, bool retry = true);

  /**
   * @brief Read a packet from the e12 device.
   * @return e12_packet_t* Pointer to the packet
   */
  virtual e12_packet_t* read();

  /**
   * @brief Get the log event.
   * @return e12_log_evt_t* Pointer to the log event
   */
  virtual e12_log_evt_t* get_log_evt();

  /**
   * @brief Set node authentication credentials.
   * @param auth Authentication data
   * @return int Status of the operation
   */
  virtual int set_node_auth_credentials(e12_auth_data_t* auth);

  /**
   * @brief Handle wakeup event.
   * @return int Status of the operation
   */
  virtual int on_wakeup();

  /**
   * @brief Put the device to sleep.
   * @param ms Sleep duration in milliseconds
   * @param data Additional data
   * @return int Status of the operation
   */
  virtual int sleep(uint32_t ms, void* data);
};

#endif
