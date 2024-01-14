#pragma once

#include "esp_wifi.h"
// #include <atomic>
#include <mutex>

namespace WIFI
{

class Wifi
{
public:
    enum class state_e
    {
        NOT_INITIALIZED,
        INITIALIZED,
        WAITING_FOR_CREDENTIALS,
        READY_TO_CONNECT,
        READY_TO_RECONNECT,
        CONNECTING,
        WAITING_FOR_IP,
        CONNECTED,
        DISCONNECTED,
        ERROR
    };


    
    Wifi(void);

    esp_err_t init(void);   // Set everything up
    esp_err_t begin(void);  // Start Wifi, connect, etc

    state_e get_state(void);
    const char* get_MAC(void){ return mac_add_cstr; }
private:
    void state_machine(void);

    esp_err_t _get_MAC(void);
    static char mac_add_cstr[13];
    // static std::atomic_bool first_call;
    static std::mutex first_call_mutx;
    static bool first_call;
};



}