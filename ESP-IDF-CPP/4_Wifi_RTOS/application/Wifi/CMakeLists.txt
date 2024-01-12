set(SOURCES Wifi.cpp)

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS .
                    REQUIRES freertos esp_common esp_event nvs_flash  )