#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "led.h"

/**
 * @brief Initialize the RGB LED setting per channel, Including the GPIO
 * for each color, mode and timer configuration
 * 
 */

static uint8_t LED_Initialized = 0;


uint64_t myLedPins[LED_CHANNEL_NUM] = {
    LED_RED_GPIO, LED_GREEN_GPIO, LED_BLUE_GPIO
};



static void led_pwm_Init(void);


static void led_pwm_Init(void){
    int i;

    gpio_config_t my_GPIOs = {
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE, 
    .pull_down_en = GPIO_PULLDOWN_ENABLE, 
    .intr_type = GPIO_INTR_DISABLE
    };
    for(i = 0; i < LED_CHANNEL_NUM; i++){
        my_GPIOs.pin_bit_mask = 1 << myLedPins[i];
        gpio_config(&my_GPIOs);
    }
    LED_Initialized = 1;
}


void led_wifi_app_off(void){
    if(LED_Initialized == 0){
        led_pwm_Init();
    }
    if( gpio_set_level(myLedPins[0], 0) != ESP_OK){
        printf("Error !!! led_wifi_app_off");
        while(1);
    }
    if( gpio_set_level(myLedPins[1], 0) != ESP_OK){
        printf("Error !!! led_wifi_app_off");
        while(1);
    }
    if( gpio_set_level(myLedPins[2], 0) != ESP_OK){
        printf("Error !!! led_wifi_app_off");
        while(1);
    }
}




/**
 * Color to indicate WiFi application has started.
 */
void led_wifi_app_started(void){
    if(LED_Initialized == 0){
        led_pwm_Init();
    }
    if( gpio_set_level(myLedPins[0], 1) != ESP_OK){
        printf("Error !!! led_wifi_app_started");
        while(1);
    }
    if( gpio_set_level(myLedPins[1], 0) != ESP_OK){
        printf("Error !!! led_wifi_app_started");
        while(1);
    }
    if( gpio_set_level(myLedPins[2], 0) != ESP_OK){
        printf("Error !!! led_wifi_app_started");
        while(1);
    }
}

/**
 * Color to indicate HTTP server has started.
 */
void led_http_server_started(void){
    if(LED_Initialized == 0){
        led_pwm_Init();
    }
    if( gpio_set_level(myLedPins[0], 0) != ESP_OK){
        printf("Error !!! led_http_server_started");
        while(1);
    }
    if( gpio_set_level(myLedPins[1], 1) != ESP_OK){
        printf("Error !!! led_http_server_started");
        while(1);
    }
    if( gpio_set_level(myLedPins[2], 0) != ESP_OK){
        printf("Error !!! led_http_server_started");
        while(1);
    }
}

/**
 * Color to indicate that the ESP32 is connected to an access point.
 */
void led_wifi_connected(void){
    if(LED_Initialized == 0){
        led_pwm_Init();
    }
    if( gpio_set_level(myLedPins[0], 0) != ESP_OK){
        printf("Error !!! led_wifi_connected");
        while(1);
    }
    if( gpio_set_level(myLedPins[1], 0) != ESP_OK){
        printf("Error !!! led_wifi_connected");
        while(1);
    }
    if( gpio_set_level(myLedPins[2], 1) != ESP_OK){
        printf("Error !!! led_wifi_connected");
        while(1);
    }
}

