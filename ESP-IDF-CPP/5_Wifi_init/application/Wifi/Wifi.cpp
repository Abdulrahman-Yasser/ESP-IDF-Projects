#include "Wifi.h"

namespace WIFI
{

char            Wifi::mac_add_cstr[]{};
std::mutex      Wifi::init_mutex{};
std::mutex      Wifi::connect_mutx{};
// we don't change the state unless we own that mutex
std::mutex      Wifi::state_mutx{};
Wifi::state_e   Wifi::_state{state_e::NOT_INITIALIZED};
wifi_init_config_t Wifi::wifi_init_config= WIFI_INIT_CONFIG_DEFAULT()
wifi_config_t   Wifi::wifi_config{};

constexpr static const char* ssid{"MyWifiSsid"};
constexpr static const char* password{"MyWifiPasswprd"};

esp_err_t Wifi::init(void)
{
    return _init();
}

esp_err_t Wifi::begin(void)
{
    std::lock_guard<std::mutex> guard_connect(connect_mutx);     

    esp_err_t status{ESP_OK};

    switch(_state)
    {
    case state_e::READY_TO_CONNECT:
        status = esp_wifi_connect();
        if(ESP_OK == status) 
            _state = state_e::CONNECTING;
        break;
    case state_e::CONNECTING:
    case state_e::WAITING_FOR_IP:
    case state_e::CONNECTED:
        break;
    case state_e::NOT_INITIALIZED:
    case state_e::INITIALIZED:
    case state_e::DISCONNECTED:
    case state_e::ERROR:
        status = ESP_FAIL;
        break;
    }

    return status;
}


void Wifi::event_handler(void* arg, esp_event_base_t event_base,
                        int32_t event_id, void* event_data)
{
    if(WIFI_EVENT == event_base)
    {
        wifi_event_handler(arg, event_base, event_id, event_data);
    }
    else if(IP_EVENT == event_base)
    {
        ip_event_handler(arg, event_base, event_id, event_data);
    }
    else
    {
        // TODO print unexpected event, event_base
    }
}

void Wifi::wifi_event_handler(void* arg, esp_event_base_t event_base,
                        int32_t event_id, void* event_data)
{
    if(WIFI_EVENT == event_base)
    {
        const wifi_event_t event_type{static_cast<wifi_event_t>(event_id)};
        switch(event_type)
        {
        case WIFI_EVENT_STA_START:
        // We don't change that state unless we own that mutex
        {
            std::lock_guard<std::mutex> guard(state_mutx);
            _state = state_e::READY_TO_CONNECT;
            break;
        }
        case WIFI_EVENT_STA_CONNECTED:
        {
            std::lock_guard<std::mutex> guard(state_mutx);
            _state = state_e::WAITING_FOR_IP;
            break;
        }
        default:
            // TODO stop and disconnected and other states
            break;
        }
    } 
}

void Wifi::ip_event_handler(void* arg, esp_event_base_t event_base,
                        int32_t event_id, void* event_data)
{
    if(IP_EVENT == event_base)
    {
        const ip_event_t event_type{static_cast<ip_event_t>(event_id)};

        switch(event_type)
        {
        case IP_EVENT_STA_GOT_IP:
        // We don't change that state unless we own that mutex
        {
            std::lock_guard<std::mutex> guard(state_mutx);
            _state = state_e::CONNECTED;
            break;
        }
        case IP_EVENT_STA_LOST_IP:
        {
            std::lock_guard<std::mutex> guard(state_mutx);
            _state = state_e::WAITING_FOR_IP;
            break;
        }
        default:
            // TODO IP6
            break;
        }
    }
}

//TODO be ausome

Wifi::Wifi(void)
{

    std::lock_guard<std::mutex> guard(connect_mutx);     

    if(!get_MAC()[0]){
        if (ESP_OK != _get_MAC()) esp_restart();
    }
}

esp_err_t Wifi::_init(void)
{
    std::lock_guard<std::mutex> init_guard(init_mutex);

    esp_err_t status{ESP_OK};

    std::lock_guard<std::mutex> state_guard(state_mutx);
    if(state_e::NOT_INITIALIZED == _state)
    {
        status = esp_netif_init();
        if(ESP_OK == status)
        {
            status = esp_wifi_init(&wifi_init_config);
        }
    
        if(ESP_OK == status)
        {
            status = esp_event_handler_register(WIFI_EVENT,
                                            ESP_EVENT_ANY_ID,
                                            &event_handler,
                                            nullptr);
        }
        if(ESP_OK == status)
        {
            status = esp_event_handler_register(IP_EVENT,
                                            ESP_EVENT_ANY_ID,
                                            &event_handler,
                                            nullptr);
        }
        if(ESP_OK == status)
        {
            status = esp_wifi_set_mode(WIFI_MODE_STA);  // TODO keep track of mode
        }
        if(ESP_OK == status)
        {
            const size_t ssid_len_to_copy = std::min(strlen(ssid), sizeof(wifi_config.sta.ssid));

            memcpy(wifi_config.sta.ssid, ssid, ssid_len_to_copy);

            const size_t password_len_to_copy = std::min(strlen(password), sizeof(wifi_config.sta.password));

            memcpy(wifi_config.sta.password, password, password_len_to_copy);

            wifi_config.sta.threshold.authmode= WIFI_AUTH_WPA2_PSK;
            wifi_config.sta.pmf_cfg.capable = true;
            wifi_config.sta.pmf_cfg.required = false;

            status = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
        }
        if(ESP_OK == status)
        {
            status = esp_wifi_start();
        }
        if(ESP_OK == status)
        {
            _state = state_e::READY_TO_CONNECT;
        }
    }
    else if(state_e::ERROR == _state)
    {
        _state = state_e::NOT_INITIALIZED;
    }

    return status;
}

esp_err_t Wifi::_get_MAC(void)
{
    // it never get destroyed
    uint8_t mac_byte_buffer[6]{};
    
    const esp_err_t status{esp_efuse_mac_get_default(mac_byte_buffer)};

    if(ESP_OK == status){
        snprintf(mac_add_cstr, sizeof(mac_add_cstr), "%02X%02X%02X%02X%02X%02X",
        mac_byte_buffer[0], mac_byte_buffer[1], mac_byte_buffer[2],
        mac_byte_buffer[3], mac_byte_buffer[4], mac_byte_buffer[5]);
    }
    return status;
}
}