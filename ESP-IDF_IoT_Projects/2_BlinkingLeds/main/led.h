/*
 * rgb_led.h
 *
 *  Created on: Oct 11, 2021
 *      Author: kjagu
 */

#ifndef MAIN_RGB_LED_H_
#define MAIN_RGB_LED_H_

// RGB LED GPIOs
#define LED_RED_GPIO		GPIO_NUM_16
#define LED_GREEN_GPIO		GPIO_NUM_5
#define LED_BLUE_GPIO		GPIO_NUM_4

// // LED color mix channels
#define LED_CHANNEL_NUM		3

// LED configuration

void led_wifi_app_off(void);
/**
 * Color to indicate WiFi application has started.
 */
void led_wifi_app_started(void);

/**
 * Color to indicate HTTP server has started.
 */
void led_http_server_started(void);

/**
 * Color to indicate that the ESP32 is connected to an access point.
 */
void led_wifi_connected(void);

#endif /* MAIN_RGB_LED_H_ */
