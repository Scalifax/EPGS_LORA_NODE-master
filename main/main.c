#include "runepgs.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_timer.h"
#include "esp_log.h"
static const char* TAG = "epgs-lora-node";
//extern SemaphoreHandle_t xSemaphore;
esp_timer_handle_t periodic_timer;

static void periodic_timer_callback(void* arg)
{
	printf("\nI'M IN ");
	switch(tagNgEPGS->ngState)
	{
		case 1:		printf("HELLO");							break;
		case 2:		printf("WAIT_HELLO_PGCS");					break;
		case 3:		printf("EXPOSITION");						break;
		case 4:		printf("SERVICE_OFFER");					break;
		case 5:		printf("WAIT_SERVICE_ACCEPTANCE_NOTIFY");	break;
		case 6:		printf("SUBSCRIBE_SERVICE_ACCEPTANCE");		break;
		case 7:		printf("WAIT_SERVICE_ACCEPTANCE_DELIVERY");	break;
		case 8:		printf("PUB_DATA");							break;
	}
	printf(" STATE\n");
}

uint8_t but[251];

void task_tx(void *p);
void task_rx(void *p);

void task_tx(void *p)
{
	startepgs();
	xTaskCreatePinnedToCore(&task_rx, "task_rx", 20480, NULL, 5, task_rx_handle,1);
	ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 2000000));
	for(;;) {
		runepgs();
		vTaskDelay(1);
	}
}

void task_rx(void *p)
{
	int x;
	for(;;)
	{
		lora_rx_mode();    // put into receive mode
		while(lora_rxed()) {
			x = lora_rx_msg(but, sizeof(but));
			newReceivedMessage(&tagNgEPGS, (char*)but,  x);
		}
		vTaskDelay(1);
	}
}

void app_main()
{
	//teste timer
	const esp_timer_create_args_t periodic_timer_args = {
			.callback = &periodic_timer_callback,
			/* name is optional, but may help identify the timer when debugging */
			.name = "periodic"
	};

	ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
	/* The timer has been created but is not running yet */

	lora_init();
	lora_set_frequency(915e6);
	lora_set_bandwidth(250e3);
	lora_set_spreading_factor(7);
	lora_enable_crc();
	xSemaphore = xSemaphoreCreateMutex();
	if( xSemaphore == NULL )
	{
		ESP_LOGI(TAG, "N�o consegui criar o sem�foro :(");
	}
	else
	{
		ESP_LOGI(TAG, "Consegui criar o sem�foro :)");
	}
	xTaskCreatePinnedToCore(&task_tx, "task_tx", 60480, NULL, 5, task_tx_handle,0);
}
