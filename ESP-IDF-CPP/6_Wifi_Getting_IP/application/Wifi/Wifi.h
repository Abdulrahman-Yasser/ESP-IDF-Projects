#pragma once

#include <mutex>
#include <cstring>
#include <algorithm>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "../Nvs32/Nvs32.h"

namespace WIFI
{
class Wifi
{

public:
    enum class state_e
    {
        NOT_INITIALIZED,
        INITIALIZED,
        READY_TO_CONNECT,
        CONNECTING,
        WAITING_FOR_IP,
        CONNECTED,
        DISCONNECTED,
        ERROR
    };

    Wifi(void);
    ~Wifi(void)                     = default;
    Wifi(const Wifi&)               = default;
    Wifi(Wifi&&)                    = default;
    Wifi& operator=(const Wifi&)    = default;
    Wifi& operator=(Wifi&&)         = default;
    
    esp_err_t init(void);   // Set everything up
    esp_err_t begin(void);  // Start Wifi, connect, etc

    // return the state but noone can change it.
    constexpr static const state_e& get_state(void){ return _state;}
    constexpr static const char* get_MAC(void){ return mac_add_cstr; }
private:
    static char mac_add_cstr[13];

    static std::mutex init_mutex;
    static std::mutex connect_mutx;
    static std::mutex state_mutx;
    static NVS::Nvs my_nvs;
    static state_e _state;
    static wifi_init_config_t wifi_init_config;
    static wifi_config_t wifi_config;
    constexpr static const char* _log_tag{"WiFi"};
    constexpr static const char* ssid{"WE_BCD7E9"};
    constexpr static const char* password{"j9a10603"};

    static esp_err_t _init(void);
    void state_machine(void);
    static void event_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data);
    static void ip_event_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data);
    static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data);
    esp_err_t _get_MAC(void);
};



}