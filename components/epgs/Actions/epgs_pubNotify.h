/*
 * runhello.h
 *
 *  Created on: 30/12/2015
 *      Author: vaner
 */

#ifndef PUB_NOTIFY_H_
#define PUB_NOTIFY_H_

#include "ng_message.h"
#include "epgs_structures.h"

int actionPublicationAndNotification(NgEPGS *ngEPGS, bool isData, char* fileName, char* filePayload, unsigned long long payloadSize, NgMessage **pubNotifyMessage);

#endif /* PUB_NOTIFY_H_ */ 
