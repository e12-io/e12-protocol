set(SOURCES 
    "src/e12_protocol.cpp"
    "esp32/esp32_e12_node_protocol.cpp"
)

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "src" "."
    REQUIRES "esp_event" "arduino-esp32"
)

