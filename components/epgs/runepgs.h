/*
 * runepgs.h
 *
 *  Created on: May 19, 2018
 *      Author: Unknown
 */

#ifndef RUNEPGS_H_
#define RUNEPGS_H_
#include "epgs.h"
#include "DataStructures/epgs_structures.h"
#include "epgs_wrapper.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lora.h"

SemaphoreHandle_t xSemaphore;

enum CHPERMISSIONS
{
	FREE = 0,
	SENDING,
	RECEIVING,
};

NgEPGS *tagNgEPGS;

int dataPSize;

TaskHandle_t task_tx_handle, task_rx_handle;

int Count;

char *dataP;

void startepgs(void);

void runepgs(void);

void lora_rx_mode(void);

int lora_rxed(void);

int lora_rx_msg(uint8_t *buf, int size);

void lora_tx_msg(uint8_t *buf, int size);

uint16_t medida;

uint16_t temperatura;

uint8_t seq;

uint8_t flagstart;

uint8_t TXPermission;

uint8_t cont;

#endif /* RUNEPGS_H_ */
