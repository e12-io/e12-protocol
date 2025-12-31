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

#ifndef H_E12_SPEC
#define H_E12_SPEC

#include <stdint.h>

/**
 * @brief e12 on wire max packet size
 *
 */
#define E12_MAX_PKT_SIZE 128

/**
 * @brief e12 max payload size in bytes in a e12 packet
 *
 */
#define E12_MAX_DATA_PAYLOAD (E12_MAX_PKT_SIZE - sizeof(e12_onwire_head_t) - 8)

/**
 * @brief e12 commands
 *
 */
enum class e12_cmd_t : uint8_t {
  NONE = 0,
  /// typical ping cmd. responded with a "pong" string
  CMD_PING,
  /// used to pass authentication credential for e.g WiFi or LTE etc
  CMD_AUTH,
  /// used to request vendor specific configuration
  CMD_INFO,
  /// used to request vendor specific configuration
  CMD_CONFIG,
  /// used to either send or request vendor state from e12 node
  CMD_STATE,
  /// asked to query the e12 node for various status information
  CMD_STATUS,
  /// used to log a vendor event to e12 node for store and forward
  CMD_LOG,
  /// request current time in ms from e12 node
  CMD_TIME,
  /// request e12 node to wake vendor mcu after certain ms
  CMD_SCHEDULE_WAKEUP,
  /// typically sent by e12 node intending to go to sleep
  CMD_NODE_SLEEP,
  /// typically sent by e12 node when it powers on or wake up from sleep
  CMD_NODE_AWAKE,
  /// request initiation of OTA
  CMD_OTA,
  /// request initiation of VMCU OTA
  CMD_VMCU_OTA,
  /// set various e12 node properties e.g logmask,
  /// activating captive portal etc
  CMD_SET_NODE_PROPERTIES,
  /// initiate a debug blink of led on e12 node
  CMD_DEBUG_BLINK
};

enum class e12_release_t : uint8_t {
  /// Stable release
  STABLE = 0,
  /// Canary release
  CANARY,
  /// Development release
  DEV
};

/**
 * @brief Used to indicate the error status of an operation.
 *
 */
enum class e12_err_t : int8_t {
  ERR_NONE = 0,
  ERR_RETRY_LATER = -1,
};

/**
 * @brief Used to indicate the status of an event.
 * This information is typically carried in a log event
 *
 */
enum class e12_evt_status_t : uint8_t {
  STATUS_DONE = 0,
  STATUS_NEW,
  STATUS_IN_PROCESSING,
  STATUS_IN_WAITING
};

/**
 * @brief Used to indicate the operation status of an e12 node
 *  e.g active, sleep, ota etc
 */
enum class e12_node_op_status_t : uint8_t {
  STATUS_NONE = 0,
  STATUS_ACTIVE,
  STATUS_SLEEP,
  STATUS_OTA,
  STATUS_VMCU_OTA
};

/**
 * @brief Supported MCU architecture
 */
enum class mcu_arch_t : uint8_t {
  ARCH_NONE = 0,
  ARCH_ATMEGA328,
  ARCH_SAMD21
};

/**
 * @brief Supported MCU flashing protocols architecture
 */
enum class mcu_flashing_protocol_t : uint8_t {
  PROTOCOL_NONE = 0,
  PROTOCOL_STK500,
  PROTOCOL_BOSSA
};

#define E12_MAX_LOG_BUFFERS 1
#define MAX_S_LOG_DATA 16
typedef struct __attribute__((packed, aligned(4))) e12_log_evt {
  uint8_t type;
  uint8_t status;
  uint8_t src;
  uint8_t src_index;
  uint32_t ts;
  uint32_t count;
  union {
    uint32_t state;
    struct {
      uint8_t s : 1;
      uint8_t f : 1;
      uint8_t i : 1;
      uint8_t : 0;
      uint8_t in_use : 1;
      uint8_t : 0;
      uint8_t resv1;
      uint8_t resv2;
    };
  };
  union {
    char s_data[MAX_S_LOG_DATA];
    struct {
      float f_data;
      int32_t i_data;
      uint32_t resv3;
      uint32_t resv4;
    };
  };
} e12_log_evt_t;

typedef struct __attribute__((packed, aligned(4))) e12_wakeup_data {
  uint32_t ms;
} e12_wakeup_data_t;

typedef struct __attribute__((packed, aligned(4))) e12_debug_blink {
  uint32_t on_ms;
  uint32_t off_ms;
  uint32_t count;
} e12_debug_blink_t;

typedef struct __attribute__((packed, aligned(4))) e12_node_properties {
  union {
    uint32_t flags;
    struct {
      uint8_t REBOOT : 1;
      uint8_t FACTORY_RESET : 1;
      uint8_t DISABLE_SLEEP : 1;
      uint8_t REFRESH_CONFIG : 1;
      uint8_t ACTIVATE_WIFI : 1;
      uint8_t ACTIVATE_LTE : 1;
      uint8_t ACTIVATE_LORA : 1;
      uint8_t ACTIVATE_WIFI_CAPTIVE_PORTAL : 1;
      uint8_t ENCRYPT : 1;
      uint8_t TRANSMIT : 1;
      uint8_t LOGMASK : 1;
    };
  };
  union {
    uint64_t data;
    struct {
      uint32_t data1;
      uint32_t data2;
    };
  };
} e12_node_properties_t;

#define MAX_SSID_LEN 32
#define MAX_PWD_LEN 32
typedef struct __attribute__((packed, aligned(4))) e12_auth_data {
  struct {
    uint8_t AUTH_WIFI : 1;
    uint8_t AUTH_LTE_AP : 1;
  };
  union {
    struct {
      char ssid[MAX_SSID_LEN];
      char pwd[MAX_PWD_LEN];
    } wifi;
  };
} e12_auth_data_t;

typedef struct __attribute__((packed, aligned(4))) e12_node_state {
  struct {
    uint8_t AUTH_OK : 1;
    uint8_t CONFIGURED : 1;
    uint8_t CONNECTED : 1;
  };
  e12_node_op_status_t op_status;
  uint16_t next_connection_in_sec;
  union {
    uint32_t data;
    struct {
      uint32_t node_wake_up_ms;  // if e12_node is in sleep mode
    };
  };
} e12_node_state_t;

#define MAX_LOG_SIZE (sizeof(e12_log_evt_t))

typedef struct __attribute__((packed, aligned(4))) e12_header {
  uint8_t seq;
  uint8_t len;
  struct {
    uint8_t RESP_EXPECTED : 1;
    uint8_t IS_RESPONSE : 1;
    uint8_t : 0;
  };
  e12_cmd_t cmd;
} e12_header_t;

#define E12_MAGIC_MARKER_LEN 2
#define E12_MAGIC_MARKER_1 (0xEC)
#define E12_MAGIC_MARKER_2 (0xEC ^ 0xCE)
typedef union __attribute__((packed, aligned(4))) e12_onwire_head {
  uint32_t head;
  struct {
    uint8_t magic[E12_MAGIC_MARKER_LEN];
    uint8_t len;
    uint8_t checksum;
  };
} e12_onwire_head_t;

#define E12_MAX_CMD_DATA_PAYLOAD (E12_MAX_DATA_PAYLOAD - sizeof(e12_header_t))
#define E12_MAX_FIRMWARE_VERSION_LEN 32

typedef union __attribute__((packed, aligned(4))) e12_packet {
  uint8_t buf[E12_MAX_DATA_PAYLOAD];
  struct {
    e12_header_t head;
    char data[E12_MAX_CMD_DATA_PAYLOAD];
  } msg;
  struct {
    e12_header_t head;
    uint32_t err;  // 0 = OK, 1 = ERR
  } msg_err;
  struct {
    e12_header_t head;
    uint32_t ms;
  } msg_wakeup;
  struct {
    e12_header_t head;
    uint32_t ms;
  } msg_sleep;
  struct {
    e12_header_t head;
    e12_debug_blink_t data;
  } msg_debug_blink;
  struct {
    e12_header_t head;
    e12_node_properties_t props;
  } msg_node_props;
  struct {
    e12_header_t head;
    e12_auth_data_t data;
  } msg_auth_credentials;
  struct {
    e12_header_t head;
    uint32_t ms;
  } msg_time;
  struct {
    e12_header_t head;
    uint32_t release_type;
    uint32_t size;
    char version[E12_MAX_FIRMWARE_VERSION_LEN];
  } msg_ota;
  struct {
    e12_header_t head;
    uint32_t version;
    mcu_arch_t arch;
    mcu_flashing_protocol_t protocol;
    bool flashing_enabled;
  } msg_info;
  struct {
    e12_header_t head;
  } msg_vmcu_ota;
} e12_packet_t;

typedef union __attribute__((packed, aligned(4))) e12_onwire {
  uint8_t buf[E12_MAX_PKT_SIZE];
  struct {
    e12_onwire_head_t head;
    e12_packet_t data;
    uint8_t resp_pending : 1;
    uint8_t : 0;
    uint8_t recv_len;
    uint8_t resv;
    uint32_t ts;
  };
} e12_onwire_t;

typedef struct __attribute__((packed, aligned(4))) e12_data {
  uint8_t IS_JSON : 1;
  uint8_t STORE : 1;
  uint8_t FETCH : 1;
  uint8_t : 0;
  uint32_t ts_ms;
  uint8_t data[E12_MAX_CMD_DATA_PAYLOAD - 5];
} e12_data_t;

typedef struct __attribute__((packed, aligned(4))) e12_device {
  e12_data_t state;
  e12_data_t config;
  e12_log_evt_t log;
} e12_device_t;

/**
 * @class e12
 * @brief This class represents the base class for the e12 protocol.
 *
 * The e12 class provides methods for encoding and decoding packets,
 * managing device state, handling communication, and performing utility
 * functions. It serves as the base class for specific implementations
 * like e12_esp32_node.
 */
class e12 {
 private:
  uint32_t _vid;             ///< Vendor ID
  uint32_t _pid;             ///< Product ID
  uint32_t _version;         ///< Version of the e12 protocol
  e12_node_state_t _status;  ///< Status of the e12 node
  uint32_t _mcu_fwr_version;         ///< Version of the e12 protocol
  mcu_arch_t _arch;
  mcu_flashing_protocol_t _protocol;
  bool _mcu_flashing_enabled;

  e12_onwire_t _encode_buf;  ///< Buffer for encoding packets
  e12_onwire_t _decode_buf;  ///< Buffer for decoding packets
  e12_device_t* _dev_ptr;    ///< Pointer to the e12 device

 protected:
  uint32_t _timeout;  ///< Timeout value in milliseconds
  uint8_t _seq;       ///< Sequence number for packets

  /**
   * @brief Gets the buffer for encoding packets.
   * @return Pointer to the encoding buffer
   */
  e12_onwire_t* get_encode_buffer() { return &_encode_buf; }

  /**
   * @brief Gets the buffer for decoding packets.
   * @return Pointer to the decoding buffer
   */
  e12_onwire_t* get_decode_buffer() { return &_decode_buf; }

  /**
   * @brief Flushes the given buffer.
   * @param buf Pointer to the buffer to be flushed
   */
  void flush_buffer(e12_onwire_t* buf);
  /**
   * @brief Gets the status of the e12 node.
   * @return Status of the e12 node
   */
  e12_node_op_status_t get_node_status();
  /**
   * @brief Sets the status of the e12 node.
   * @param status Status to be set
   * @param data uint32_t size data to be stored relevant to the status e.g for
   * SLEEP this is ms till the e12_node is in sleep mode
   * @return Status of the e12 node
   */
  e12_node_op_status_t set_node_status(e12_node_op_status_t status,
                                       uint32_t data);

 public:
  /**
   * @brief Constructor for the e12 class.
   * @param vid Vendor ID
   * @param pid Product ID
   */
  e12(uint32_t vid, uint32_t pid);

  /**
   * @brief Destructor for the e12 class.
   */
  ~e12();

  // Packet handling

  /**
   * @brief Gets a new packet for the e12 protocol.
   * @return Pointer to the new packet
   */
  e12_packet_t* e12_get_packet();

  /**
   * @brief Encodes the given data into a packet.
   * @param data Pointer to the data to be encoded
   * @return Pointer to the encoded packet
   */
  e12_onwire_t* encode(e12_packet_t* data);

  /**
   * @brief Decodes the given data into a packet.
   * @param pkt Pointer to the packet to be decoded
   * @param data Data to be decoded
   * @return Pointer to the decoded packet
   */
  e12_packet_t* decode(e12_onwire_t* pkt, uint8_t data);

  // Device management

  /**
   * @brief Sets the e12 device.
   * @param p Pointer to the e12 device
   */
  void set_e12_device(e12_device_t* p) { _dev_ptr = p; }

  /**
   * @brief Sets the product information.
   * @param vid Vendor ID
   * @param pid Product ID
   */
  void set_product_info(uint32_t vid, uint32_t pid) {
    _vid = vid;
    _pid = pid;
  }

  /**
   * @brief Set the vmcu firmware details object
   * 
   * @param arch 
   * @param protocol 
   * @param enabled 
   */
  void set_fwr_details(uint32_t fwr_version, mcu_arch_t arch,
                      mcu_flashing_protocol_t protocol, bool enabled) {
    _mcu_fwr_version = fwr_version;
    _arch = arch;
    _protocol = protocol;
    _mcu_flashing_enabled = enabled;
  }

  /**
   * @brief Get the fwr version object
   * 
   * @return uint32_t 
   */
  uint32_t get_fwr_version() {
    return _mcu_fwr_version;
  }

  /**
   * @brief Publish info e.g fwr version, arch, protocol etc
   *
   * @return int
   */
  int publish_info();
  
  /**
   * @brief Sets the properties of the e12 node.
   * @param props Pointer to the node properties
   */
  void set_node_properties(e12_node_properties_t* props);

  /**
   * @brief Sets the timeout value.
   * @param ms Timeout value in milliseconds
   */
  void set_timeout(uint32_t ms) { _timeout = ms; }

  /**
   * @brief Checks if the e12 endpoint is configured.
   * @return True if the endpoint is configured, false otherwise
   */
  bool is_configured() { return _status.CONFIGURED; }

  /**
   * @brief Set the configured status for e12 endpoint
   * @param status  true if configured, false otherwise
   */
  void set_configured(bool status) { _status.CONFIGURED = status; }

  /**
   * @brief Gets the version of the e12 protocol.
   * @return Version of the e12 protocol
   */
  uint32_t get_version() { return _version; }

  /**
   * @brief Sets the version of the e12 protocol.
   * @param v Version to be set
   */
  void set_version(uint32_t v) { _version = v; }

  // Communication

  /**
   * @brief Gets a request packet for the given command.
   * @param cmd Command to be requested
   * @param response True if a response is expected, false otherwise
   * @param data Pointer to additional data
   * @return Pointer to the request packet
   */
  virtual e12_packet_t* get_request(e12_cmd_t cmd, bool response = true,
                                    void* data = 0);

  /**
   * @brief Gets a response packet for the given packet.
   * @param p Pointer to the packet
   * @return Pointer to the response packet
   */
  virtual e12_packet_t* get_response(e12_packet_t* p);

  /**
   * @brief Gets a message from the e12 protocol.
   * @param data Pointer to the data to be retrieved
   * @return True if the message was retrieved, false otherwise
   */
  bool get_message(e12_packet_t* data);

  /**
   * @brief Get the checksum object
   *
   * @param data Pointer to the data
   * @param len Length of the data
   * @return uint8_t Returns the checksum
   */
  uint8_t get_checksum(const char* data, uint8_t len);

  /**
   * @brief Handles the received packet.
   * @param p Pointer to the received packet
   * @return 0 on success, non-zero on failure
   */
  virtual int on_receive(e12_packet_t* p);

  /**
   * @brief Wakes up the e12 node.
   * @return 0 on success, non-zero on failure
   */
  virtual int wakeup_e12_node() { return 0; }

  // Utility

  /**
   * @brief Prints the contents of the buffer.
   * @param buf Pointer to the buffer
   * @return 0 on success, non-zero on failure
   */
  virtual int print_buffer(e12_onwire_t* buf) { return 0; }

  // Pure virtual functions to be implemented by derived classes

  virtual int begin(void* bus, uint8_t e12_addr = 0) = 0;
  virtual uint32_t get_time_ms() = 0;
  virtual e12_log_evt_t* get_log_evt() = 0;
  virtual int send(e12_packet_t* buf, bool retry = true) = 0;
  virtual e12_packet_t* read() = 0;
  virtual int sleep(uint32_t ms, void* data) = 0;
  virtual int log(uint8_t type, uint8_t status, uint32_t ts, void* data) = 0;
  virtual int on_wakeup() = 0;
  virtual int set_node_auth_credentials(e12_auth_data_t* auth) = 0;
  virtual int on_config(const char* s, int len) = 0;
  virtual int on_get_state(char* s, int len, void* ctx) = 0;
  virtual int on_restore_state(const char* s, int len) = 0;
};

#endif
