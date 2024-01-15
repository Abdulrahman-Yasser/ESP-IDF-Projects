/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "main.h"


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
	ESP_LOGE(_log_tag,"Hello\nIt is set-up !!\n");
	ESP_LOGE(_log_tag,"Guarding the mutex");
	std::lock_guard<std::mutex> guarding(my_mutex); 
	ESP_LOGE(_log_tag,"Guarded mutex");
	Led.init();
	return status;
}

void Main::run(void){
	ESP_LOGE(_log_tag,"LED on");
	Led.set(true);
	vTaskDelay(1000 / portTICK_PERIOD_MS);

	ESP_LOGE(_log_tag,"LED off");
	Led.set(false);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
}