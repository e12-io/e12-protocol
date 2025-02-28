/*
  e12_protocol.h - on wire protocol spec for e12 protocol to talk to e12 node.
  for more information see e12.io/docs
  Created by Devesh Kothari, Feb 28, 2025.
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
  CMD_WAKE_ME_UP,
  /// request initiation of OTA [currently not implemented]
  CMD_OTA,
  /// set various e12 node properties e.g logmask,
  /// activating captive portal etc
  CMD_SET_NODE_PROPERTIES,
  /// initiate a debug blink of led on e12 node
  CMD_DEBUG_BLINK
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
  uint8_t resv;
  uint16_t next_connection_in_sec;
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
    uint8_t resv;
  };
} e12_onwire_head_t;

#define E12_MAX_CMD_DATA_PAYLOAD (E12_MAX_DATA_PAYLOAD - sizeof(e12_header_t))

typedef union __attribute__((packed, aligned(4))) e12_packet {
  uint8_t buf[E12_MAX_DATA_PAYLOAD];
  struct {
    e12_header_t head;
    char data[E12_MAX_CMD_DATA_PAYLOAD];
  } msg;
  struct {
    e12_header_t head;
    uint32_t ms;
  } msg_wakeup;
  struct {
    e12_header_t head;
    e12_debug_blink_t data;
  } msg_debug_blink;
  struct {
    e12_header_t head;
    e12_node_properties_t data;
  } msg_node_props;
  struct {
    e12_header_t head;
    e12_auth_data_t data;
  } msg_auth_credentials;
  struct {
    e12_header_t head;
    uint32_t ms;
  } msg_time;
} e12_packet_t;

typedef union __attribute__((packed, aligned(4))) e12_onwire {
  uint8_t buf[E12_MAX_PKT_SIZE];
  struct {
    e12_onwire_head_t head;
    e12_packet_t data;
    uint8_t checksum;
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
  uint8_t data[E12_MAX_CMD_DATA_PAYLOAD - 1];
} e12_data_t;

typedef struct __attribute__((packed, aligned(4))) e12_device {
  e12_data_t state;
  e12_data_t config;
  e12_log_evt_t log;
} e12_device_t;

class e12 {
 private:
  uint32_t _vid;
  uint32_t _pid;
  uint32_t _version;
  e12_node_state_t _status;

  e12_onwire_t _encode_buf;
  e12_onwire_t _decode_buf;
  e12_device_t* _dev_ptr;

 protected:
  uint32_t _timeout;
  uint8_t _seq;
  e12_onwire_t* get_encode_buffer() { return &_encode_buf; }
  e12_onwire_t* get_decode_buffer() { return &_decode_buf; }
  void flush_buffer(e12_onwire_t* buf);

 public:
  e12(uint32_t vid, uint32_t pid);
  ~e12();
  e12_packet_t* e12_get_packet();
  e12_onwire_t* encode(e12_packet_t* data);
  e12_packet_t* decode(e12_onwire_t* pkt, uint8_t data);
  void set_e12_device(e12_device_t* p) { _dev_ptr = p; }
  bool get_message(e12_packet_t* data);
  void set_timeout(uint32_t ms) { _timeout = ms; };
  bool is_configured() { return _status.CONFIGURED; }
  uint32_t get_version() { return _version; }
  void set_version(uint32_t v) { _version = v; }
  void set_node_properties(e12_node_properties_t* props);
  void set_product_info(uint32_t vid, uint32_t pid) {
    _vid = vid;
    _pid = pid;
  }

  virtual e12_packet_t* get_request(e12_cmd_t cmd, bool response = true,
                                    void* data = 0);
  virtual e12_packet_t* get_response(e12_packet_t* p);
  virtual int print_buffer(e12_onwire_t* buf) { return 0; };
  virtual int on_receive(e12_packet_t* p);
  virtual int wakeup_e12() { return 0; };

  virtual int begin(void* bus, uint8_t e12_addr = 0) = 0;
  virtual uint32_t get_time_ms() = 0;
  virtual e12_log_evt_t* get_log_evt() = 0;
  virtual uint8_t get_checksum(const char* data, uint8_t len) = 0;
  virtual int send(e12_packet_t* buf) = 0;
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
