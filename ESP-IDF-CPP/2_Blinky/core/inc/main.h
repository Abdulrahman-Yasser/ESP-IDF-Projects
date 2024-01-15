#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_event.h"
#include "esp_log.h"
#include <mutex>

#include "Gpio.h"

class Main final{
    public :
        constexpr static const char* _log_tag{"WiFi"};
        Gpio::GpioOutput Led{GPIO_NUM_4, true};
        esp_err_t setup(void);
        void run(void);
    private:
        std::mutex my_mutex{};

};