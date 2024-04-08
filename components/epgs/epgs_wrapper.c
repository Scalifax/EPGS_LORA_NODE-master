/*
 * epgs_wrapper.c
 *
 *  Created on: 17/02/2016
 *      Author: Vaner Jos� Magalh�es
 *		E-mail: vaner@inatel.br
 */
#include "epgs_wrapper.h"
//#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "runepgs.h"

//#include "Ethernet.h"

//#include "PhysicalLayer/WiFi/WiFi.h"

int ng_rand (void) {
	srand ( time(NULL) );
	return rand();
}

int ng_atoi (const char* str) {
	return atoi(str);
}

unsigned long ng_strtoul (const char* str, char** endptr, int base) {
	return strtoul (str, endptr, base);
}

void* ng_calloc (long unsigned int nitems, long unsigned int size) {
	return calloc(nitems, size);
}

void* ng_malloc (long unsigned int size) {
	return pvPortMalloc(size);
	//return malloc(size);
}

void* ng_realloc (void* ptr, long unsigned int size) {
	return realloc(ptr, size);
}

void ng_free (void* ptr) {
	vPortFree(ptr);
	//free(ptr);
}

void* ng_memcpy (void* ptrDst, const void* ptrSrc, long unsigned int size) {
	return memcpy (ptrDst, ptrSrc, size);
}

int ng_strcmp (const char* ptrA, const char* ptrB) {
	return strcmp(ptrA, ptrB);
}

char* ng_strcpy (char* ptrDst, const char* ptrSrc) {
	return strcpy (ptrDst, ptrSrc);
}

long unsigned int ng_strlen (const char* str) {
	return strlen(str);
}

char* ng_strncpy (char* ptrDst, const char* ptrSrc, long unsigned int n) {
	return strncpy(ptrDst, ptrSrc, n);
}

char* ng_strcat (char* ptrDst, const char* ptrSrc) {
	return strcat (ptrDst, ptrSrc);
}

long unsigned int ng_strspn (const char* str1, const char* str2) {
	return strspn (str1, str2);
}

long unsigned int ng_strcspn (const char* str1, const char* str2) {
	return strcspn (str1, str2);
}

int ng_printf (const char* format, ...) {
	register int retval = 0;

	__builtin_va_list args;
	__builtin_va_start(args, format);
	retval = vprintf(format, args);
	__builtin_va_end(args);
	return retval;
}

int ng_sprintf (char* str, const char* format, ...) {
	register int retval;
	__builtin_va_list args;
	__builtin_va_start(args, format);
	retval = vsprintf(str, format, args);
	__builtin_va_end(args);
	return retval;
}

double ng_GetTime()
{
	return 1000;
}

void ng_EthernetSendData(void* addr, long long size) {


	//Ethernet_sendData(addr, size);

	//WiFi_writePacket(addr, size);
}

void ng_LoRaSendData(char* addr, unsigned int size) {

//	//char TDataBuff[128];
//	//lora_AppData_t TData = {TDataBuff, 0, 0};
//	//sprintf((char*)TXData.Buff, "%s", addr);
//	for(int contagem = 0; contagem<200; contagem++)
//	{
//		TXDataBuff[contagem] = addr[contagem];
//	}
//	//TXData.Buff = addr;
//	TXData.BuffSize = (uint8_t)size;
//	TXData.Port = 10;
//	//LORA_send(&TData, LORAWAN_CONFIRMED_MSG);
//	//TXLoRa(&TXData);
//	LORA_send(&TXData, LORAWAN_CONFIRMED_MSG);
//	vTaskDelay(pdMS_TO_TICKS(1000));
	lora_tx_msg((uint8_t*)addr, size);
}

void ng_BLESendData(char* addr, long long size) {


	//EPGSMessage(addr, size);
}
