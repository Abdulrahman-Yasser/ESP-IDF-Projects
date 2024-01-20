/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "main.h"

#include "nvs.h"
#include "nvs_flash.h"


static Main my_main;


extern "C" void app_main()
{
	my_main.setup();

	while(true){
		my_main.loop();
	}
}



esp_err_t Main::setup(void){
	esp_err_t status{ESP_OK};
	ESP_LOGE("MAIN", "Calling nvs_flash_init() from main");
	nvs_flash_init();
	ESP_LOGE("MAIN", "Called nvs_flash_init() from main");
	printf("Hello\nIt is set-up !!\n");
	status |= Led.init();
	status |= my_wifi.init();
	nvs_flash_init();
	if(ESP_OK == status)
		status |= my_wifi.begin();
	if(ESP_OK == status)
		status |= sntp.init();
	return status;
}

void Main::loop(void){
	ESP_LOGI("MAIN", "LED on");
	Led.set(true);
	vTaskDelay(1000 / portTICK_PERIOD_MS);

	ESP_LOGI("MAIN", "LED off");
	Led.set(false);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
}


// TODO know how tf lock_guard works, because it lock, return, then continue working.