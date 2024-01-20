#pragma once
#include <chrono>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "Gpio.h"
#include "Wifi.h"
#include "SntpTime.h"

class Main final{
    public :
        
        esp_err_t setup(void);
        void loop(void);
        Gpio::GpioOutput Led{GPIO_NUM_4, true};
        SNTP::Sntp& sntp{SNTP::Sntp::get_instance()}; 
        WIFI::Wifi my_wifi{};
};


