#pragma once

#include "driver/gpio.h"

namespace Gpio
{

class GpioBase
{
protected:
    const gpio_num_t _pin;
    const bool _inverted_logic = false;
    const gpio_config_t _cfg;

public:
    /* const gpio_config_t to trigger the copy constructer,
    if it wasn't const it will trigger the move constuctor.
    Copy is kinda bad because it takes time and memory 

    constexpr is to do all of that at compile time, 
    which will be acceptable rather than doing this at run time.
    */
    constexpr GpioBase(const gpio_num_t pin, 
            const gpio_config_t& config, 
            const bool inverted_logic = false):
        _pin{pin},
        _inverted_logic{inverted_logic},
        _cfg{config}
    {

    }

    virtual bool state(void) = 0;
    virtual esp_err_t set(const bool state) = 0;
    [[nodiscard]] esp_err_t init(void);
};

class GpioOutput : public GpioBase
{
private:
    bool _state = false;
public:
    constexpr GpioOutput(const gpio_num_t pin, const bool invert = false):
        GpioBase{pin,
                gpio_config_t{
                    .pin_bit_mask   = static_cast<uint32_t>(1) << pin,
                    .mode           = GPIO_MODE_OUTPUT,
                    .pull_up_en     = GPIO_PULLUP_DISABLE,
                    .pull_down_en   = GPIO_PULLDOWN_ENABLE,
                    .intr_type      = GPIO_INTR_DISABLE
                }, 
                invert}
    {
    
    }
    esp_err_t toggle(void);
    [[nodiscard]] esp_err_t init(void);
    esp_err_t set(const bool state);
    bool state(void){return _state;}
};

}