/**
 * Programa de Testes do EPGS
 */

#include "epgs_wrapper.h"
#include "Common/ng_hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include "runepgs.h"
#include <string.h>
#include <stdint.h>

SemaphoreHandle_t xSemaphore = NULL;
NgEPGS *tagNgEPGS = NULL;
int dataPSize = 0;
TaskHandle_t task_tx_handle = NULL;
TaskHandle_t task_rx_handle = NULL;
int Count = 0;
char *dataP = NULL;
uint16_t medida = 0;
uint16_t temperatura = 0;
uint8_t seq = 0;
uint8_t flagstart = 0;
uint8_t TXPermission = 0;
uint8_t cont = 0;

const char ucIdentify[] = {"e0:e2:e6:00:71:0c"}; //GW_Station
const char ucHID[] = {"12345"};
const char ucSOID[] = {"NG_SO"};
const char ucName[] = {"Measures_B_"};
const char ucPID[] = {"4321"};
const char cTemp[] = {"{ Temperature_B_"};

const char ucStack[] = {"Wi-Fi"};
const char ucInterface[] = {"eth1"};
const char cSensorType[] = {"Temperature"};

void startepgs()
{
	/*Init epgs*/
	initEPGS(&tagNgEPGS);

	setHwConfigurations(&tagNgEPGS, &ucHID, &ucSOID, &ucStack, &ucInterface, &ucIdentify);
	//setCycleDuration(&tagNgEPGS, 10);
	addKeyWords(&tagNgEPGS, cSensorType);
	addHwSensorFeature(&tagNgEPGS, "sensorType", "Temperature");
	addHwSensorFeature(&tagNgEPGS, "sensorRangeMin", "-20");
	addHwSensorFeature(&tagNgEPGS, "sensorRangeMax", "100");
	addHwSensorFeature(&tagNgEPGS, "sensorResolution", "1");
	addHwSensorFeature(&tagNgEPGS, "sensorAccuracy", "2");
}

void runepgs(void)
{
	processLoop(&tagNgEPGS);
}

void lora_rx_mode(void)
{
	if( xSemaphore != NULL )
	{
		// See if we can obtain the semaphore.  If the semaphore is not available
		// wait 10 ticks to see if it becomes free.
		if( xSemaphoreTake( xSemaphore, portMAX_DELAY ) == pdTRUE )
		{
			// We were able to obtain the semaphore and can now access the
			// shared resource.
			lora_receive();    // put into receive mode
			// We have finished accessing the shared resource.  Release the
			// semaphore.
			xSemaphoreGive( xSemaphore );
		}
		else
		{
			// We could not obtain the semaphore and can therefore not access
			// the shared resource safely.
		}
	}
}

int lora_rxed(void)
{
	int ans=0;
	if( xSemaphore != NULL )
	{
		// See if we can obtain the semaphore.  If the semaphore is not available
		// wait 10 ticks to see if it becomes free.
		if( xSemaphoreTake( xSemaphore, portMAX_DELAY ) == pdTRUE )
		{
			// We were able to obtain the semaphore and can now access the
			// shared resource.
			ans = lora_received();	//verify if has received msg
			// We have finished accessing the shared resource.  Release the
			// semaphore.
			xSemaphoreGive( xSemaphore );
		}
		else
		{
			// We could not obtain the semaphore and can therefore not access
			// the shared resource safely.
		}
	}
	return ans;
}

int lora_rx_msg(uint8_t *buf, int size)
{
	int ans=0;
	if( xSemaphore != NULL )
	{
		// See if we can obtain the semaphore.  If the semaphore is not available
		// wait 10 ticks to see if it becomes free.
		if( xSemaphoreTake( xSemaphore, portMAX_DELAY ) == pdTRUE )
		{
			// We were able to obtain the semaphore and can now access the
			// shared resource.
			ans = lora_receive_packet(buf, size);	//	Receive the msg
			// We have finished accessing the shared resource.  Release the
			// semaphore.
			xSemaphoreGive( xSemaphore );
		}
		else
		{
			// We could not obtain the semaphore and can therefore not access
			// the shared resource safely.
		}
	}
	return ans;
}

void lora_tx_msg(uint8_t *buf, int size)
{
	if( xSemaphore != NULL )
	{
		// See if we can obtain the semaphore.  If the semaphore is not available
		// wait 10 ticks to see if it becomes free.
		if( xSemaphoreTake( xSemaphore, portMAX_DELAY ) == pdTRUE )
		{
			// We were able to obtain the semaphore and can now access the
			// shared resource.
			lora_send_packet(buf, size);
			// We have finished accessing the shared resource.  Release the
			// semaphore.
			xSemaphoreGive( xSemaphore );
		}
		else
		{
			// We could not obtain the semaphore and can therefore not access
			// the shared resource safely.
		}
	}
}
