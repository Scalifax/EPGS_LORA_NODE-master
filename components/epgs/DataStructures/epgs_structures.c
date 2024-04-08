/*
 * epgs_structures.c
 *
 *  Created on: 05/02/2016
 *      Author: vaner
 */
#include "epgs_structures.h"
#include "epgs_defines.h"
#include "epgs_wrapper.h"

void destroy_NgNetInfo(struct _ng_net_info **ngHWInfo) {
	if((*ngHWInfo) != NULL) {
		if((*ngHWInfo)->Stack != NULL) {
			ng_free((*ngHWInfo)->Stack);
			(*ngHWInfo)->Stack = NULL;
		}
		if((*ngHWInfo)->Interface != NULL) {
			ng_free((*ngHWInfo)->Interface);
			(*ngHWInfo)->Interface = NULL;
		}
		if((*ngHWInfo)->Identifier != NULL) {
			ng_free((*ngHWInfo)->Identifier);
			(*ngHWInfo)->Identifier = NULL;
		}
		ng_free((*ngHWInfo));
		(*ngHWInfo) = NULL;
	}
}

void destroy_NgHwDescriptor(struct _ng_hw_descriptor **ngHwDescriptor) {
	if((*ngHwDescriptor) != NULL) {
		int i = 0;

		if((*ngHwDescriptor)->keyWords) {
			for (i=0; i<(*ngHwDescriptor)->keyWordsCounter; i++){
				if((*ngHwDescriptor)->keyWords[i]) {
					ng_free((*ngHwDescriptor)->keyWords[i]);
					(*ngHwDescriptor)->keyWords[i] = NULL;
				}
			}
			ng_free((*ngHwDescriptor)->keyWords);
			(*ngHwDescriptor)->keyWords = NULL;
		}


		if((*ngHwDescriptor)->sensorFeatureName) {
			for (i=0; i<(*ngHwDescriptor)->featureCounter; i++){
				if((*ngHwDescriptor)->sensorFeatureName[i]) {
					ng_free((*ngHwDescriptor)->sensorFeatureName[i]);
					(*ngHwDescriptor)->sensorFeatureName[i] = NULL;
				}
			}
			ng_free((*ngHwDescriptor)->sensorFeatureName);
			(*ngHwDescriptor)->sensorFeatureName = NULL;
		}
		if((*ngHwDescriptor)->sensorFeatureValue) {
			for (i=0; i<(*ngHwDescriptor)->featureCounter; i++){
				if((*ngHwDescriptor)->sensorFeatureValue[i]) {
					ng_free((*ngHwDescriptor)->sensorFeatureValue[i]);
					(*ngHwDescriptor)->sensorFeatureValue[i] = NULL;
				}
			}
			ng_free((*ngHwDescriptor)->sensorFeatureValue);
			(*ngHwDescriptor)->sensorFeatureValue = NULL;
		}
		(*ngHwDescriptor)->featureCounter = 0;
		ng_free((*ngHwDescriptor));
		(*ngHwDescriptor) = NULL;
	}
}

void destroy_NgPGCSInfo(struct _ng_pgcs_info **ngPeerInfo) {

	if((*ngPeerInfo) != NULL) {
		if((*ngPeerInfo)->Stack != NULL) {
			ng_free((*ngPeerInfo)->Stack);
			(*ngPeerInfo)->Stack = NULL;
		}
		if((*ngPeerInfo)->Interface != NULL) {
			ng_free((*ngPeerInfo)->Interface);
			(*ngPeerInfo)->Interface = NULL;
		}
		if((*ngPeerInfo)->Identifier != NULL) {
			ng_free((*ngPeerInfo)->Identifier);
			(*ngPeerInfo)->Identifier = NULL;
		}
		ng_free((*ngPeerInfo));
		(*ngPeerInfo) = NULL;
	}
}

void destroy_NgScnIDInfo(struct _ng_scn_id_info **ngScnIDInfo) {
	if((*ngScnIDInfo) != NULL) {
		ng_free((*ngScnIDInfo));
		(*ngScnIDInfo) = NULL;
	}
}

void destroy_NgReceivedMsg(struct _ng_received_msg **ngReceivedMsg) {
	if(*ngReceivedMsg != NULL) {
		if((*ngReceivedMsg)->buffer != NULL) {
			ng_free((*ngReceivedMsg)->buffer);
			(*ngReceivedMsg)->buffer = NULL;
		}
		ng_free(*ngReceivedMsg);
		(*ngReceivedMsg) = NULL;
	}
}

void destroy_NgEPGS(struct _ng_epgs **ngEPGS) {
	if((*ngEPGS) != NULL) {
		destroy_NgNetInfo(&(*ngEPGS)->NetInfo);
		destroy_NgPGCSInfo(&(*ngEPGS)->PGCSNetInfo);
		destroy_NgScnIDInfo(&(*ngEPGS)->PGCSScnIDInfo);
		destroy_NgScnIDInfo(&(*ngEPGS)->PSSScnIDInfo);
		destroy_NgScnIDInfo(&(*ngEPGS)->APPScnIDInfo);
		destroy_NgReceivedMsg(&(*ngEPGS)->ReceivedMsg);
		destroy_NgHwDescriptor(&(*ngEPGS)->HwDescriptor);

		if((*ngEPGS)->key) {
			ng_free((*ngEPGS)->key);
			(*ngEPGS)->key = NULL;
		}

		if((*ngEPGS)->pubData) {
			ng_free((*ngEPGS)->pubData);
			(*ngEPGS)->pubData = NULL;
		}

		if((*ngEPGS)->pubDataFileName) {
			ng_free((*ngEPGS)->pubDataFileName);
			(*ngEPGS)->pubDataFileName = NULL;
		}

		ng_free((*ngEPGS));
		(*ngEPGS) = NULL;
	}
}
