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
        CONNECTING,
        WAITING_FOR_IT,
        CONNECTED,
        DISCONNECTED,
        ERROR
    };

    Wifi(void){
        /* 
        we are the only thread running that line of code once we go 
        out of scope it get released, and the blocked thread will run it.
        */
        std::lock_guard<std::mutex> guard(first_call_mutx);     
        // if(!first_call){
        if(!first_call){
            if (ESP_OK != _get_MAC()) esp_restart();
            first_call = true; 
        }
    }

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