/*
 * EPGS_Controller.h
 *
 *  Created on: 03/02/2016
 *      Author: vaner
 */

#ifndef CONTROLLER_EPGS_CONTROLLER_H_
#define CONTROLLER_EPGS_CONTROLLER_H_

#include "epgs.h"

int RunHello(NgEPGS *ngEPGS);
int RunExposition(NgEPGS *ngEPGS);
int RunPubServiceOffer(NgEPGS *ngEPGS);
int RunSubscribeServiceAcceptance(NgEPGS *ngEPGS);
int RunPublishData(NgEPGS *ngEPGS);
int ParseReceivedMessage(NgEPGS **ngEPGS);

#endif /* CONTROLLER_EPGS_CONTROLLER_H_ */
