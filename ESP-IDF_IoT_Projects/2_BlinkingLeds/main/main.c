/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "led.h"





void app_main()
{
	while(1){
		led_wifi_app_off();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		led_wifi_app_started();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		led_http_server_started();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		led_wifi_connected();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
