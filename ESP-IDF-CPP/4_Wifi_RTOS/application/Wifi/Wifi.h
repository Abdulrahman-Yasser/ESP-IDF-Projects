#pragma once

#include "esp_wifi.h"
// #include <atomic>
#include "freertos/FreeRTOS.h"

#include "freertos/queue.h"
#include <freertos/task.h>
#include "freertos/semphr.h"
#include "nvs_flash.h"
#include <esp_log.h>

#define pdSECOND pdMS_TO_TICKS(1000)


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


    esp_err_t init(void);   // Set everything up
    esp_err_t begin(void);  // Start Wifi, connect, etc
    Wifi(void);
    state_e get_state(void);
    const char* get_MAC(void){ return mac_add_cstr; }
private:
    void state_machine(void);

    esp_err_t _get_MAC(void);
    static char mac_add_cstr[13];
    // static std::atomic_bool first_call;
    static SemaphoreHandle_t first_call_mutx;
    static bool first_call;
};



}