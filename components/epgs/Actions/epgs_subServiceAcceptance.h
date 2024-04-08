/*
 * subServiceAcceptance.h
 *
 *  Created on: 30/12/2015
 *      Author: vaner
 */

#ifndef SUB_SERVICE_ACCEPTANCE_H_
#define SUB_SERVICE_ACCEPTANCE_H_

#include "ng_message.h"
#include "epgs_structures.h"

int ActionSubscriptionServiceAcceptance(NgNetInfo *hwInfo, NgScnIDInfo *pgcsSCNInfo, NgScnIDInfo *pssSCNInfo, int seqNumber, char* key, NgMessage **subServiceAcceptanceMessage);

#endif /* SUB_SERVICE_ACCEPTANCE_H_ */ 
