#include "Gpio.h"

namespace Gpio{
    [[nodiscard]] esp_err_t GpioBase::init(void)
    {
        esp_err_t status{ESP_OK};
        status |= gpio_config(&_cfg);
        return status;
    }

    [[nodiscard]] esp_err_t GpioOutput::init(void){
        esp_err_t status{GpioBase::init()};
        if(ESP_OK == status)
        {
            status |= set(this->_inverted_logic);
        }
        return status;
    }

    esp_err_t GpioOutput::set(const bool state)
    {
        this->_state = state;


        return gpio_set_level(  this->_pin,
                                this->_inverted_logic ? !state : state);
    }

    esp_err_t GpioOutput::toggle(void)
    {
        return set( !this->_state);
    }
}
