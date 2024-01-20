#include "Wifi.h"

namespace WIFI
{

char            Wifi::mac_add_cstr[]{};

/* Mutexes */
std::mutex      Wifi::init_mutex{};
std::mutex      Wifi::connect_mutx{};
// we don't change the state unless we own that mutex
std::mutex      Wifi::state_mutx{};


Wifi::state_e   Wifi::_state{state_e::NOT_INITIALIZED};

wifi_init_config_t Wifi::wifi_init_config= WIFI_INIT_CONFIG_DEFAULT()
wifi_config_t   Wifi::wifi_config{};


//TODO be ausome

Wifi::Wifi(void)
{
    ESP_LOGE(_log_tag, "%s:%d WIFI constructor", __func__, __LINE__);
    // std::lock_guard<std::mutex> guard_state(connect_mutx);
    // ESP_LOGE(_log_tag, "%s:%d Got MUTEX", __func__, __LINE__);
    if(!get_MAC()[0]){
        ESP_LOGE(_log_tag, "%s:%d Getting MAC", __func__, __LINE__);
        if (ESP_OK != _get_MAC()) esp_restart();
    }
    ESP_LOGE(_log_tag, "%s:%d I guess i got MAC", __func__, __LINE__);
}


esp_err_t Wifi::init(void)
{
    return _init();
}

esp_err_t Wifi::begin(void)
{
    ESP_LOGE(_log_tag, "%s:%d Waiting for connect_mutex : \n", __func__, __LINE__);
    

    ESP_LOGE(_log_tag, "%s:%d Waiting for state_mutex : \n", __func__, __LINE__);
    std::lock_guard<std::mutex> guard_state(state_mutx);     

    esp_err_t status{ESP_OK};

    switch(_state)
    {
    case state_e::READY_TO_CONNECT:
        ESP_LOGE(_log_tag, "%s:%d Calling esp_wifi_connect() : \n", __func__, __LINE__);
        status = esp_wifi_connect();
        ESP_LOGE(_log_tag, "%s:%d Calling esp_wifi_connect() :%s \n", __func__, __LINE__, esp_err_to_name(status));
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
        ESP_LOGE(_log_tag, "%s:%d Calling esp_wifi_connect() :%s \n", __func__, __LINE__, esp_err_to_name(status));
        status = ESP_FAIL;
        break;
    }

    return status;
}


void Wifi::event_handler(void* arg, esp_event_base_t event_base,
                        int32_t event_id, void* event_data)
{
    ESP_LOGE(_log_tag, "%s:%d Got an EVENT\n", __func__, __LINE__);
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
        ESP_LOGE(_log_tag, "%s:%d Got a IP_EVENT, Unexpected event: %s\n", __func__, __LINE__, event_base);
    }
}

void Wifi::wifi_event_handler(void* arg, esp_event_base_t event_base,
                        int32_t event_id, void* event_data)
{
    if(WIFI_EVENT == event_base)
    {
        const wifi_event_t event_type{static_cast<wifi_event_t>(event_id)};
        ESP_LOGE(_log_tag, "%s:%d Event ID : %d\n", __func__, __LINE__, event_id);
        switch(event_type)
        {
        case WIFI_EVENT_STA_START:
        // We don't change that state unless we own that mutex
        {
            ESP_LOGE(_log_tag, "%s:%d STA_START, waiting for state_mutex : \n", __func__, __LINE__);
            std::lock_guard<std::mutex> guard(state_mutx);
            _state = state_e::READY_TO_CONNECT;
            ESP_LOGE(_log_tag, "%s:%d READY_TO_CONNECT : \n", __func__, __LINE__);
            break;
        }
        case WIFI_EVENT_STA_CONNECTED:
        {
            ESP_LOGE(_log_tag, "%s:%d WIFI_EVENT_STA_CONNECTED, waiting for state_mutex : \n", __func__, __LINE__);
            std::lock_guard<std::mutex> guard(state_mutx);
            _state = state_e::WAITING_FOR_IP;
            ESP_LOGE(_log_tag, "%s:%d Waiting for IP : \n", __func__, __LINE__);
            break;
        }
        default:
            ESP_LOGE(_log_tag, "%s:%d Default switch case (%d) : \n", __func__, __LINE__, event_id);
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
            ESP_LOGE(_log_tag, "%s:%d Got IP, waiting for state_mutex : %d\n", __func__, __LINE__, event_id);
            std::lock_guard<std::mutex> guard(state_mutx);
            _state = state_e::CONNECTED;
            ESP_LOGE(_log_tag, "%s:%d CONNECTED! : \n", __func__, __LINE__);
            break;
        }
        case IP_EVENT_STA_LOST_IP:
        {
            ESP_LOGE(_log_tag, "%s:%d Lost IP, waiting for state_mutex : \n", __func__, __LINE__);
            std::lock_guard<std::mutex> guard(state_mutx);
            _state = state_e::WAITING_FOR_IP;
            ESP_LOGE(_log_tag, "%s:%d D, waiting for IP :\n", __func__, __LINE__);
            break;
        }
        default:
            ESP_LOGE(_log_tag, "%s:%d Default switch case :\n", __func__, __LINE__);
            // TODO IP6
            break;
        }
    }
}




esp_err_t Wifi::_init(void)
{
    ESP_LOGE(_log_tag, "%s:%d Waiting for init_mutex : \n", __func__, __LINE__);
    std::lock_guard<std::mutex> init_guard(init_mutex);

    esp_err_t status{ESP_OK};

    ESP_LOGE(_log_tag, "%s:%d Waiting for state_mutex : \n", __func__, __LINE__);
    std::lock_guard<std::mutex> state_guard(state_mutx);
    if(state_e::NOT_INITIALIZED == _state)
    {
        ESP_LOGE(_log_tag, "%s:%d Calling :tcpip_adapter_init() \n", __func__, __LINE__);
        tcpip_adapter_init();
        nvs_flash_init();
        if(ESP_OK == status)
        {
            status |= esp_event_loop_create_default();
        }
        if(ESP_OK == status)
        {
            ESP_LOGE(_log_tag, "%s:%d Calling :esp_wifi_init() \n", __func__, __LINE__);
            status = esp_wifi_init(&wifi_init_config);
        }
    
        if(ESP_OK == status)
        {
            ESP_LOGE(_log_tag, "%s:%d Calling :esp_event_handler_register() \n", __func__, __LINE__);
            status = esp_event_handler_register(WIFI_EVENT,
                                            ESP_EVENT_ANY_ID,
                                            &event_handler,
                                            nullptr);
            ESP_LOGE(_log_tag, "%s:%d returned from :esp_event_handler_register(), status is %s \n", __func__, __LINE__,
                                                                                esp_err_to_name(status));
        }
        if(ESP_OK == status)
        {
            ESP_LOGE(_log_tag, "%s:%d Calling :esp_event_handler_register() \n", __func__, __LINE__);
            status = esp_event_handler_register(IP_EVENT,
                                            IP_EVENT_STA_GOT_IP,
                                            &event_handler,
                                            nullptr);
            ESP_LOGE(_log_tag, "%s:%d returned from :esp_event_handler_register(), status is %s \n", __func__, __LINE__,
                                                                                esp_err_to_name(status));
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

            ESP_LOGE(_log_tag, "%s:%d Calling :esp_wifi_set_config() and  esp_wifi_set_config()\n", __func__, __LINE__);
            status |= esp_wifi_set_mode(WIFI_MODE_STA);  // TODO keep track of mode
            status |= esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
            ESP_LOGE(_log_tag, "%s:%d returned from :esp_wifi_set_config() and esp_wifi_set_config(), status is %s \n", __func__, __LINE__,
                                                                                esp_err_to_name(status));
        }
        if(ESP_OK == status)
        {
            ESP_LOGE(_log_tag, "%s:%d Calling :esp_wifi_start() \n", __func__, __LINE__);
            status |= esp_wifi_start();
            ESP_LOGE(_log_tag, "%s:%d returned from :esp_wifi_start(), status is %s \n", __func__, __LINE__,
                                                                                esp_err_to_name(status));
        }
        if(ESP_OK == status)
        {
            ESP_LOGE(_log_tag, "%s:%d Initialized \n", __func__, __LINE__);
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
    ESP_LOGE(_log_tag, "%s:%d Priniting MAC", __func__, __LINE__);
    if(ESP_OK == status){
        int retVal = snprintf(mac_add_cstr, sizeof(mac_add_cstr), "%02X%02X%02X%02X%02X%02X",
        mac_byte_buffer[0], mac_byte_buffer[1], mac_byte_buffer[2],
        mac_byte_buffer[3], mac_byte_buffer[4], mac_byte_buffer[5]);

        ESP_LOGE(_log_tag, "%02X%02X%02X%02X%02X%02X"
                            , mac_add_cstr[0],
                            mac_add_cstr[1], 
                            mac_add_cstr[2], 
                            mac_add_cstr[3], 
                            mac_add_cstr[4], 
                            mac_add_cstr[5]);
    }
    return status;
}
}