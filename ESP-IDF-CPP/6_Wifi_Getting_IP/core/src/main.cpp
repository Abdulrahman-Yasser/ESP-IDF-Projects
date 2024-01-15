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
		my_main.run();
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
	{
		status |= my_wifi.begin();
	}else
	{
		ESP_LOGE("MAIN","Problem in init \n");
	}
	return status;
}

void Main::run(void){
	printf("LED on");
	Led.set(true);
	vTaskDelay(1000 / portTICK_PERIOD_MS);

	printf("LED off");
	Led.set(false);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
}