set(SOURCES Wifi.cpp)

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS .
                    REQUIRES freertos spi_flash pthread nvs_flash esp_event lwip)