#include "epgs_controller.h"
#include "epgs_structures.h"
#include "epgs_wrapper.h"
#include "epgs_hello.h"
#include "epgs_pubNotify.h"
#include "epgs_subServiceAcceptance.h"
#include "epgs_exposition.h"
#include "epgs_defines.h"
#include "ng_command.h"
#include "ng_message.h"
#include "ng_util.h"
#include "ng_json.h"
#include "PG.h"

int RunHello(NgEPGS *ngEPGS) {

	if(!ngEPGS) {
		return NG_ERROR;
	}
	if(!(ngEPGS->NetInfo)) {
		return NG_ERROR;
	}

	NgMessage *helloMessage = NULL;
	ActionRunHello(ngEPGS->NetInfo, &helloMessage);

	//ng_printf("%s", helloMessage->Msg);
	sendNGMessage(ngEPGS, helloMessage, true);

	ng_destroy_message(&helloMessage);

	return NG_OK;
}

int RunExposition(NgEPGS *ngEPGS) {
	int result = NG_ERROR;

	if(!ngEPGS) {
		return result;
	}
	if(!ngEPGS->NetInfo) {
		return result;
	}
	if(!ngEPGS->PSSScnIDInfo) {
		return result;
	}

	NgMessage *expositioneMessage = NULL;
	result = actionExpostion(ngEPGS, &expositioneMessage);

//	ng_printf("%s", expositioneMessage->Msg);

	if(result == NG_OK) {
		result = sendNGMessage(ngEPGS, expositioneMessage, false);
	}
	ng_destroy_message(&expositioneMessage);

	return result;
}

int RunPubServiceOffer(NgEPGS *ngEPGS) {
	if(!ngEPGS) {
		return NG_ERROR;
	}
	if(!ngEPGS->NetInfo) {
		return NG_ERROR;
	}
	if(!ngEPGS->PGCSNetInfo) {
		return NG_ERROR;
	}
	if(!ngEPGS->PGCSScnIDInfo) {
		return NG_ERROR;
	}
	if(!ngEPGS->PSSScnIDInfo) {
		return NG_ERROR;
	}

	char* filePayload = (char*)ng_calloc(sizeof(char), 600);
	ng_strcpy(filePayload, "ng -sr --b 0.1 [ < 1 s 17 > < 1 s EPGS_Sensor > ");

	ng_sprintf(filePayload + ng_strlen(filePayload), "< %i s ", ngEPGS->HwDescriptor->featureCounter);
	int i = 0;
	for(i = 0; i < ngEPGS->HwDescriptor->featureCounter; i++) {
		ng_sprintf(filePayload + ng_strlen(filePayload), "%s ", ngEPGS->HwDescriptor->sensorFeatureName[i]);
	}
	ng_sprintf(filePayload + ng_strlen(filePayload), "> ]\n");

	for(i = 0; i < ngEPGS->HwDescriptor->featureCounter; i++) {
		ng_sprintf(filePayload + ng_strlen(filePayload), "ng -sr --b 0.1 [ < 1 s 17 > < 1 s %s > < 1 s %s > ]\n", ngEPGS->HwDescriptor->sensorFeatureName[i], ngEPGS->HwDescriptor->sensorFeatureValue[i]);
	}

	NgMessage *pubServiceOfferMessage = NULL;
	actionPublicationAndNotification(ngEPGS, false, "Service_Offer.txt", filePayload, (unsigned long long)ng_strlen(filePayload), &pubServiceOfferMessage);
	ng_free(filePayload);

//	ng_printf("%s", pubServiceOfferMessage->Msg);
	sendNGMessage(ngEPGS, pubServiceOfferMessage, false);

	ng_destroy_message(&pubServiceOfferMessage);

	return NG_OK;

}

int RunSubscribeServiceAcceptance(NgEPGS *ngEPGS) {
	int result = NG_ERROR;

	if(!ngEPGS) {
		return result;
	}
	if(!ngEPGS->NetInfo) {
		return result;
	}
	if(!ngEPGS->PGCSScnIDInfo) {
		return result;
	}
	if(!ngEPGS->PSSScnIDInfo) {
		return result;
	}

	NgMessage *subServiceAcceptanceMessage = NULL;
	result = ActionSubscriptionServiceAcceptance(ngEPGS->NetInfo, ngEPGS->PGCSScnIDInfo, ngEPGS->PSSScnIDInfo, ngEPGS->MessageCounter, ngEPGS->key, &subServiceAcceptanceMessage);

	if(result == NG_OK) {
		result = sendNGMessage(ngEPGS, subServiceAcceptanceMessage, false);
	}
	ng_destroy_message(&subServiceAcceptanceMessage);

	return result;
}

int RunPublishData(NgEPGS *ngEPGS) {
	if(!ngEPGS) {
		return NG_ERROR;
	}
	if(!ngEPGS->PGCSNetInfo) {
		return NG_ERROR;
	}
	if(!ngEPGS->PGCSScnIDInfo) {
		return NG_ERROR;
	}
	if(!ngEPGS->PSSScnIDInfo) {
		return NG_ERROR;
	}
	if(!ngEPGS->pubData) {
		return NG_ERROR;
	}

	NgMessage *pubDataMessage = NULL;
	actionPublicationAndNotification(ngEPGS, true, ngEPGS->pubDataFileName, ngEPGS->pubData, ngEPGS->pubDataSize, &pubDataMessage);

//	ng_printf("%s", pubDataMessage->Msg);

	sendNGMessage(ngEPGS, pubDataMessage, false);
	ng_destroy_message(&pubDataMessage);

	return NG_OK;

}

int ParseReceivedMessage(NgEPGS **ngEPGS) {

	int result = NG_OK;
	bool updateSCNs = false;

	NgPGCSInfo* pgcsInfo = NULL;
	NgScnIDInfo* pgcsSCNInfo = NULL;
	NgScnIDInfo* pssSCNInfo = NULL;


	ng_printf("\n<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-");
	ng_printf("\nNOVAGENESIS MSG RECEIVED:");
	ng_printf("\n%s", (*ngEPGS)->ReceivedMsg->buffer);
	ng_printf("\n<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-");

	NgMessage *ngMessage;
	MessageFromString((*ngEPGS)->ReceivedMsg->buffer, (*ngEPGS)->ReceivedMsg->mgs_size, &ngMessage);


	int i = 0;
	for(i = 0; i< ngMessage->NoCL; i++) {
		NgCommand *CL = NULL;
		GetCommandLine(ngMessage, i, &CL);

		if(ng_strcmp("-m",CL->Name)==0 && ng_strcmp("--cl",CL->Alternative)==0) {

			if(CL->NoA == 3) {
				unsigned int nEle;
				GetNumberofArgumentElements(CL, 1, &nEle);
				if(nEle == 4 ){
					pgcsSCNInfo = (NgScnIDInfo*) ng_malloc(sizeof(NgScnIDInfo));

					char *value;

					GetArgumentElement(CL, 1, 0, &value);
					ng_strcpy(pgcsSCNInfo->HID, value);
					ng_free(value);

					GetArgumentElement(CL, 1, 1, &value);
					ng_strcpy(pgcsSCNInfo->OSID, value);
					ng_free(value);

					GetArgumentElement(CL, 1, 2, &value);
					ng_strcpy(pgcsSCNInfo->PID, value);
					ng_free(value);

					GetArgumentElement(CL, 1, 3, &value);
					ng_strcpy(pgcsSCNInfo->BID, value);
					ng_free(value);
				}
				 else {
				result = NG_ERROR;
				}
			} else {
				result = NG_ERROR;
			}
		}

		else if(ng_strcmp("-hello",CL->Name)==0 && ng_strcmp("--ihc",CL->Alternative)==0 && ng_strcmp("0.2",CL->Version)==0) {

			if(CL->NoA == 2) {
				unsigned int nEle;
				GetNumberofArgumentElements(CL, 0, &nEle);

				if(nEle == 6 ){
					pgcsInfo = (NgPGCSInfo*) ng_malloc(sizeof(NgPGCSInfo));

					char *value;

					GetArgumentElement(CL, 0, 0, &value);
					ng_strcpy(pgcsInfo->GW_SCN, value);
					ng_free(value);

					GetArgumentElement(CL, 0, 1, &value);
					ng_strcpy(pgcsInfo->HT_SCN, value);
					ng_free(value);

					GetArgumentElement(CL, 0, 2, &value);
					ng_strcpy(pgcsInfo->CORE_BID_SCN, value);
					ng_free(value);

					GetArgumentElement(CL, 0, 3, &value);
					pgcsInfo->Stack = (char*) ng_calloc(sizeof(char), ng_strlen(value)+1);
					ng_strcpy(pgcsInfo->Stack, value);
					ng_free(value);

					GetArgumentElement(CL, 0, 4, &value);
					pgcsInfo->Interface = (char*) ng_calloc(sizeof(char), ng_strlen(value)+1);
					ng_strcpy(pgcsInfo->Interface, value);
					ng_free(value);

					GetArgumentElement(CL, 0, 5, &value);
					pgcsInfo->Identifier = (char*) ng_calloc(sizeof(char), ng_strlen(value)+1);
					ng_strcpy(pgcsInfo->Identifier, value);
					ng_free(value);

					updateSCNs = true;
				}
				 else {
				result = NG_ERROR;
				}

				GetNumberofArgumentElements(CL, 1, &nEle);

				if(nEle == 4 ){
					pssSCNInfo = (NgScnIDInfo*) ng_malloc(sizeof(NgScnIDInfo));

					char *value;

					GetArgumentElement(CL, 1, 0, &value);
					ng_strcpy(pssSCNInfo->HID, value);
					ng_free(value);

					GetArgumentElement(CL, 1, 1, &value);
					ng_strcpy(pssSCNInfo->OSID, value);
					ng_free(value);

					GetArgumentElement(CL, 1, 2, &value);
					ng_strcpy(pssSCNInfo->PID, value);
					ng_free(value);

					GetArgumentElement(CL, 1, 3, &value);
					ng_strcpy(pssSCNInfo->BID, value);
					ng_free(value);
				}
				 else {
				result = NG_ERROR;
				}
			} else {
				result = NG_ERROR;
			}

		}

		else if(ng_strcmp("-notify",CL->Name)==0 && ng_strcmp("--s",CL->Alternative)==0 ) {

			updateSCNs = false;

			if(CL->NoA == 3) {
				unsigned int nEle;
				GetNumberofArgumentElements(CL, 1, &nEle);

				if(nEle == 1 ) {

					if((*ngEPGS)->key) {
						ng_free((*ngEPGS)->key);
						(*ngEPGS)->key = NULL;
					}
					GetArgumentElement(CL, 1, 0, &(*ngEPGS)->key);

					if((*ngEPGS)->ngState == WAIT_SERVICE_ACCEPTANCE_NOTIFY)(*ngEPGS)->ngState = SUBSCRIBE_SERVICE_ACCEPTANCE;


					updateSCNs = false;
				}
				 else {
					result = NG_ERROR;
				}


			} else {
				result = NG_ERROR;
			}
		}

		else if(ng_strcmp("-d",CL->Name)==0 && ng_strcmp("--b",CL->Alternative)==0 ) {

			updateSCNs = false;

			if(CL->NoA == 3) {
				unsigned int nEle;
				GetNumberofArgumentElements(CL, 1, &nEle);

				if(nEle == 1 ) {
					char *key;
					GetArgumentElement(CL, 1, 0, &key);

					if(ng_strcmp((*ngEPGS)->key, key) == 0)
					{
						if((*ngEPGS)->APPScnIDInfo) {
							destroy_NgScnIDInfo(&(*ngEPGS)->APPScnIDInfo);
						}
						(*ngEPGS)->APPScnIDInfo = (NgScnIDInfo*) ng_malloc(sizeof(NgScnIDInfo));

						char *end_str = NULL;
						char* auxStr = (char*)ng_calloc(sizeof(char), (ngMessage->PayloadSize +1));
						ng_memcpy(auxStr, ngMessage->Payload, ngMessage->PayloadSize);

						char* token = strtok_r(auxStr, " ", &end_str);

						if(token) {
							ng_strcpy((*ngEPGS)->APPScnIDInfo->HID, token);
							token = strtok_r(NULL, " ", &end_str);
						}
						else {
							result = NG_ERROR;
						}

						if(token) {
							ng_strcpy((*ngEPGS)->APPScnIDInfo->OSID, token);
							token = strtok_r(NULL, " ", &end_str);
						}
						else {
							result = NG_ERROR;
						}

						if(token) {
							ng_strcpy((*ngEPGS)->APPScnIDInfo->PID, token);
							token = strtok_r(NULL, " ", &end_str);
						}
						else {
							result = NG_ERROR;
						}

						if(token) {
							ng_strcpy((*ngEPGS)->APPScnIDInfo->BID, token);
						//	ng_free(token);
						}
						else {
							result = NG_ERROR;
						}

						if(result == NG_OK) {
							(*ngEPGS)->ngState = PUB_DATA;
						}
					}

					ng_free(key);
					updateSCNs = false;
				}
				else {
					result = NG_ERROR;
				}


			} else {
				result = NG_ERROR;
			}
		}
		CL=NULL;
	}

	if(updateSCNs == true) {
		if((*ngEPGS)->PGCSNetInfo) {
			destroy_NgPGCSInfo(&(*ngEPGS)->PGCSNetInfo);
		}
		if((*ngEPGS)->PGCSScnIDInfo) {
			destroy_NgScnIDInfo(&(*ngEPGS)->PGCSScnIDInfo);
		}
		if((*ngEPGS)->PSSScnIDInfo) {
			destroy_NgScnIDInfo(&(*ngEPGS)->PSSScnIDInfo);
		}

		(*ngEPGS)->PGCSNetInfo = pgcsInfo;
		(*ngEPGS)->PGCSScnIDInfo = pgcsSCNInfo;
		(*ngEPGS)->PSSScnIDInfo = pssSCNInfo;
	} else {

		if(pgcsInfo) {
			destroy_NgPGCSInfo(&pgcsInfo);
		}
		if(pgcsSCNInfo) {
			destroy_NgScnIDInfo(&pgcsSCNInfo);
		}
		if(pssSCNInfo) {
			destroy_NgScnIDInfo(&pssSCNInfo);
		}
	}

	ng_destroy_message(&ngMessage);

	return result;
}
