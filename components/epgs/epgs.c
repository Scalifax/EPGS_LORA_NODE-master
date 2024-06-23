/*
===============================================================================
 Name        : EPGS.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
 */

#include "epgs.h"
#include "epgs_wrapper.h"
#include "Common/ng_epgs_hash.h"
#include "Controller/epgs_controller.h"
#include "Network/PG.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "runepgs.h"
#include "string.h"

int initEPGS(NgEPGS **ngEPGS) {

	if(*ngEPGS) {
		return NG_ERROR;
	}

	(*ngEPGS) = (NgEPGS*) ng_malloc(sizeof(NgEPGS));

	(*ngEPGS)->NetInfo = (NgNetInfo*) ng_malloc(sizeof(NgNetInfo));

	(*ngEPGS)->PGCSNetInfo = NULL;

	(*ngEPGS)->PGCSScnIDInfo = NULL;

	(*ngEPGS)->PSSScnIDInfo = NULL;

	(*ngEPGS)->APPScnIDInfo = NULL;

	(*ngEPGS)->ReceivedMsg = NULL;

	(*ngEPGS)->HwDescriptor = (NgHwDescriptor*) ng_malloc (sizeof(NgHwDescriptor));
	(*ngEPGS)->HwDescriptor->keyWords = NULL;
	(*ngEPGS)->HwDescriptor->keyWordsCounter = 0;

	(*ngEPGS)->HwDescriptor->sensorFeatureName = NULL;
	(*ngEPGS)->HwDescriptor->sensorFeatureValue = NULL;
	(*ngEPGS)->HwDescriptor->featureCounter = 0;

	addKeyWords(ngEPGS, "EPGS");
	addKeyWords(ngEPGS, "Embedded_Proxy_Gateway_Service");

	(*ngEPGS)->MessageCounter = 0;
	(*ngEPGS)->ngState = HELLO;
	(*ngEPGS)->HelloCounter = 0;

	(*ngEPGS)->pubDataFileName = NULL;
	(*ngEPGS)->pubDataSize = 0;
	(*ngEPGS)->pubData = NULL;

	(*ngEPGS)->key = NULL;

	return NG_OK;
}

int destroyEPGS(NgEPGS **ngEPGS) {
	destroy_NgEPGS(ngEPGS);
	return 1;
}

int setHwConfigurations(NgEPGS **ngEPGS, const char* hid, const char* soid, const char* Stack, const char* Interface, const char* Identifier) {
	if(!(*ngEPGS)->NetInfo) {
			return NG_ERROR;
		}

		ng_strcpy((*ngEPGS)->NetInfo->HID, hid);
		ng_strcpy((*ngEPGS)->NetInfo->SOID, soid);

		(*ngEPGS)->NetInfo->Stack = (char*) ng_malloc(sizeof(char) * (ng_strlen(Stack) + 1));
		ng_strcpy((*ngEPGS)->NetInfo->Stack, Stack);

		(*ngEPGS)->NetInfo->Interface = (char*) ng_malloc(sizeof(char) * (ng_strlen(Interface) + 1));
		ng_strcpy((*ngEPGS)->NetInfo->Interface, Interface);

		(*ngEPGS)->NetInfo->Identifier = (char*) ng_malloc(sizeof(char) * (ng_strlen(Identifier) + 1));
		ng_strcpy((*ngEPGS)->NetInfo->Identifier, Identifier);

		return NG_OK;
}

int processLoop(NgEPGS **ngEPGS) {

//	if((*ngEPGS)->ngState == 1 || (*ngEPGS)->ngState == 8)
//	{
//	if((*ngEPGS)->LoopCounter * (*ngEPGS)->MilliSecondsPerLoop < (*ngEPGS)->MilliSecondsPerCycle) {
//		(*ngEPGS)->LoopCounter++;
//		return NG_OK;
//	}
//	}
//
//	(*ngEPGS)->LoopCounter = 0;

	char ucFile[100];
	char ucBufferName[200];
	switch((*ngEPGS)->ngState) {
		case HELLO:
			if((*ngEPGS)->HelloCounter >= 5 ) {
				(*ngEPGS)->ngState = WAIT_HELLO_PGCS;
				(*ngEPGS)->HelloCounter = 0;
			}
			(*ngEPGS)->HelloCounter++;
//			vTaskDelay( 5000/portTICK_PERIOD_MS );
			RunHello((*ngEPGS));
			if((*ngEPGS)->PGCSNetInfo && (*ngEPGS)->PGCSScnIDInfo && (*ngEPGS)->PSSScnIDInfo) {
				(*ngEPGS)->ngState = EXPOSITION;
			}
			break;

		case WAIT_HELLO_PGCS:
			//Moved to the HELLO state, causing the exposition when reaches a PGCS hello
//			if((*ngEPGS)->PGCSNetInfo && (*ngEPGS)->PGCSScnIDInfo && (*ngEPGS)->PSSScnIDInfo) {
//				(*ngEPGS)->ngState = EXPOSITION;
//			}
			break;

		case EXPOSITION:
//			vTaskDelay( 5000/portTICK_PERIOD_MS );
			if(RunExposition((*ngEPGS)) == NG_OK) {
				(*ngEPGS)->ngState = SERVICE_OFFER;
				cont = 0;
			}
			break;

		case SERVICE_OFFER:
//			vTaskDelay( 5000/portTICK_PERIOD_MS );
			if (cont < 5) {
				if(RunPubServiceOffer((*ngEPGS)) == NG_OK) {
					(*ngEPGS)->ngState = WAIT_SERVICE_ACCEPTANCE_NOTIFY;
					cont++;
				}
			}
			break;

		case WAIT_SERVICE_ACCEPTANCE_NOTIFY:
//			vTaskDelay( 5000/portTICK_PERIOD_MS );
			if (cont < 5) {
				(*ngEPGS)->ngState = SERVICE_OFFER;
			}
			//IDLE TO RECEIBE A POSSIBLE SERVICE ACCEPTANCE RESPONSE
			//NG ONLY LEAVE THIS STATE WHEN IT RECEIVES A SERVICE ACCEPTANCE NOTIFY MESSAGE
			break;

		case SUBSCRIBE_SERVICE_ACCEPTANCE:
//			vTaskDelay( 5000/portTICK_PERIOD_MS );
			if (cont < 10) {
				if(RunSubscribeServiceAcceptance((*ngEPGS)) == NG_OK) {
					(*ngEPGS)->ngState = WAIT_SERVICE_ACCEPTANCE_DELIVERY;
					cont++;
				}
			}
			break;

		case WAIT_SERVICE_ACCEPTANCE_DELIVERY:
//			vTaskDelay( 5000/portTICK_PERIOD_MS );
			if (cont < 10) {
				(*ngEPGS)->ngState = SUBSCRIBE_SERVICE_ACCEPTANCE;
			}
			//NG ONLY LEAVE THIS STATE WHEN IT RECEIVES SERVICE ACCEPTANCE DELIVERY MESSAGE WITH NO ERRORS
			break;

		case PUB_DATA:
//			vTaskDelay( 5000/portTICK_PERIOD_MS );
			sprintf(ucFile, "{ Temperature: %d }", temperatura);
			sprintf(ucBufferName,"Measures_%s_%d.json", tagNgEPGS->NetInfo->HID, Count);
			setDataToPub(&tagNgEPGS, ucBufferName, ucFile, strlen(ucFile));
			Count++;
			break;
	}

	return NG_OK;
}



int newReceivedMessage(NgEPGS **ngEPGS, const char *message, int msgSize) {
	int result = newMessageReceived (ngEPGS, message, msgSize);

	if(result != NG_PROCESSING) {
//		processLoop(ngEPGS); //Let the periodic processLoop in runEPGS handle that bro
		destroy_NgReceivedMsg(&((*ngEPGS)->ReceivedMsg));
	}

	return result;
}

int addKeyWords(NgEPGS **ngEPGS, const char* key) {
	(*ngEPGS)->HwDescriptor->keyWords = (char**) ng_realloc((*ngEPGS)->HwDescriptor->keyWords, ((*ngEPGS)->HwDescriptor->keyWordsCounter + 1) * sizeof(char*));
	(*ngEPGS)->HwDescriptor->keyWords[(*ngEPGS)->HwDescriptor->keyWordsCounter] = (char*)ng_malloc(sizeof(char) * (ng_strlen(key) + 1));
	ng_strcpy((*ngEPGS)->HwDescriptor->keyWords[(*ngEPGS)->HwDescriptor->keyWordsCounter], key);

	(*ngEPGS)->HwDescriptor->keyWordsCounter++;

	return NG_OK;
}


int addHwSensorFeature(NgEPGS **ngEPGS, const char* name, const char* value) {
	(*ngEPGS)->HwDescriptor->sensorFeatureName = (char**) ng_realloc((*ngEPGS)->HwDescriptor->sensorFeatureName, ((*ngEPGS)->HwDescriptor->featureCounter + 1) * sizeof(char*));
	(*ngEPGS)->HwDescriptor->sensorFeatureName[(*ngEPGS)->HwDescriptor->featureCounter] = (char*)ng_malloc(sizeof(char) * (ng_strlen(name) + 1));
	ng_strcpy((*ngEPGS)->HwDescriptor->sensorFeatureName[(*ngEPGS)->HwDescriptor->featureCounter], name);

	(*ngEPGS)->HwDescriptor->sensorFeatureValue = (char**) ng_realloc((*ngEPGS)->HwDescriptor->sensorFeatureValue, ((*ngEPGS)->HwDescriptor->featureCounter + 1) * sizeof(char*));
	(*ngEPGS)->HwDescriptor->sensorFeatureValue[(*ngEPGS)->HwDescriptor->featureCounter] = (char*)ng_malloc(sizeof(char) * (ng_strlen(value) + 1));
	ng_strcpy((*ngEPGS)->HwDescriptor->sensorFeatureValue[(*ngEPGS)->HwDescriptor->featureCounter], value);

	(*ngEPGS)->HwDescriptor->featureCounter++;

	return NG_OK;
}


int setDataToPub(NgEPGS **ngEPGS, const char* fileName, const char* data, int dataLength) {

	if(!fileName || !data || dataLength <= 0) {
		return NG_ERROR;
	}

	(*ngEPGS)->pubDataFileName = (char*) ng_realloc((*ngEPGS)->pubDataFileName, (ng_strlen(fileName) + 1) * sizeof(char));
	(*ngEPGS)->pubData = (char*) ng_realloc((*ngEPGS)->pubData, (dataLength) * sizeof(char));

	ng_strcpy((*ngEPGS)->pubDataFileName, fileName);

	int i = 0;
	for(i = 0; i<dataLength; i++) {
		(*ngEPGS)->pubData[i] = data[i];
	}

	(*ngEPGS)->pubDataSize = dataLength;

	if((*ngEPGS)->pubData) {
		RunPublishData((*ngEPGS));
	}

	return NG_OK;
}