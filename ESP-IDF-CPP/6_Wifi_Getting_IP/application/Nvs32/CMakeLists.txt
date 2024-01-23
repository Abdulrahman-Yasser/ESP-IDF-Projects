# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(SOURCES Nvs32.cpp)

# idf_component_register(SRCS ${SOURCES}
#                     INCLUDE_DIRS .
#                     REQUIRES freertos spi_flash pthread nvs_flash esp_event lwip)

# target_compile_options(${COMPONENT_LIB} PUBLIC -std=gnu++17)