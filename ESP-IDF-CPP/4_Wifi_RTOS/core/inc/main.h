#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "Gpio.h"
#include "Wifi.h"

class Main final{
    public :
        Gpio::GpioOutput Led{GPIO_NUM_4, true};
        esp_err_t setup(void);
        void run(void);
};