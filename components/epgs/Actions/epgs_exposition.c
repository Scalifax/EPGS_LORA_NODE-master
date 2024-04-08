/*
 * exposition.c
 *
 *  Created on: 05/02/2016
 *      Author: vaner
 */
#include "epgs_exposition.h"
#include "epgs_wrapper.h"
#include "ng_message.h"
#include "ng_command.h"
#include "ng_epgs_hash.h"

extern char ucPID[];

int actionExpostion(NgEPGS *ngEPGS, NgMessage **expositionMessage) {
	int result = NG_OK;

	if(!ngEPGS || !ngEPGS->NetInfo || !ngEPGS->NetInfo->HID || !ngEPGS->NetInfo->SOID)
	{
		return NG_ERROR;
	}
	if(!ngEPGS->PSSScnIDInfo || !ngEPGS->PSSScnIDInfo->HID || !ngEPGS->PSSScnIDInfo->OSID || !ngEPGS->PSSScnIDInfo->PID || !ngEPGS->PSSScnIDInfo->BID)
	{
		return NG_ERROR;
	}

	if(!ngEPGS->HwDescriptor || ngEPGS->HwDescriptor->keyWordsCounter <= 0 || !ngEPGS->HwDescriptor->keyWords) {
		return NG_ERROR;
	}

	unsigned long long len = 0;

	len = ng_strlen(ngEPGS->NetInfo->HID);
	char* hidSCN;
	GenerateSCNFromCharArrayBinaryPatterns4Bytes(ngEPGS->NetInfo->HID, len, &hidSCN);

	len = ng_strlen(ngEPGS->NetInfo->SOID);
	char* soidSCN;
	GenerateSCNFromCharArrayBinaryPatterns4Bytes(ngEPGS->NetInfo->SOID, len, &soidSCN);

	len = 4;
	char* pidSCN;
	GenerateSCNFromCharArrayBinaryPatterns4Bytes("4321", len, &pidSCN);

	len = ng_strlen(SCN_NG_DOMAIN);
	char* limiterSCN;
	GenerateSCNFromCharArrayBinaryPatterns4Bytes(SCN_NG_DOMAIN, len, &limiterSCN);

	NgMessage *message = NULL;
	ng_create_message(ng_GetTime(), 1, true, &message);

	NgCommand* CL = ng_create_command("-m", "--cl", "0.1");
	NewArgument(CL, 1);
	SetArgumentElement(CL, 0, 0, limiterSCN);

	NewArgument(CL, 4);
	SetArgumentElement(CL, 1, 0, hidSCN);
	SetArgumentElement(CL, 1, 1, soidSCN);
	SetArgumentElement(CL, 1, 2, pidSCN);
	SetArgumentElement(CL, 1, 3, "NULL");

	ng_free(hidSCN);
	ng_free(soidSCN);

	ng_free(limiterSCN);

	NewArgument(CL, 4);
	SetArgumentElement(CL, 2, 0, ngEPGS->PSSScnIDInfo->HID);
	SetArgumentElement(CL, 2, 1, ngEPGS->PSSScnIDInfo->OSID);
	SetArgumentElement(CL, 2, 2, ngEPGS->PSSScnIDInfo->PID);
	SetArgumentElement(CL, 2, 3, ngEPGS->PSSScnIDInfo->BID);

	NewCommandLine(message, CL);

	int i = 0;
	for(i=0; i<ngEPGS->HwDescriptor->keyWordsCounter; i++) {
		if(!ngEPGS->HwDescriptor->keyWords[i]) {
			continue;
		}

		NgCommand* bindingCL = ng_create_command("-p", "--b", "0.1");
		NewArgument(bindingCL, 1);
		SetArgumentElement(bindingCL, 0, 0, "2"); // Category

		len = ng_strlen(ngEPGS->HwDescriptor->keyWords[i]);
		char* auxSCN;
		GenerateSCNFromCharArrayBinaryPatterns4Bytes(ngEPGS->HwDescriptor->keyWords[i], len, &auxSCN);

		NewArgument(bindingCL, 1);
		SetArgumentElement(bindingCL, 1, 0, auxSCN); // Key
		ng_free(auxSCN);
		NewArgument(bindingCL, 1);
		SetArgumentElement(bindingCL, 2, 0, pidSCN); // Process

		NewCommandLine(message, bindingCL);

		NgCommand* bindingNlnCL = ng_create_command("-p", "--b", "0.1");
		NewArgument(bindingNlnCL, 1);
		SetArgumentElement(bindingNlnCL, 0, 0, "1"); // Category

		NewArgument(bindingNlnCL, 1);
		SetArgumentElement(bindingNlnCL, 1, 0, ngEPGS->HwDescriptor->keyWords[i]); // Key
		NewArgument(bindingNlnCL, 1);
		SetArgumentElement(bindingNlnCL, 2, 0, pidSCN); // Process

		NewCommandLine(message, bindingNlnCL);
	}

	char number[20];
	NgCommand* msgTypeCL = ng_create_command("-message", "--type", "0.1");
	NewArgument(msgTypeCL, 1);
	ng_sprintf(number, "%d", message->Type);
	SetArgumentElement(msgTypeCL, 0, 0, number);
	NewCommandLine(message, msgTypeCL);

	NgCommand* seqNumberCL = ng_create_command("-message", "--seq", "0.1");
	NewArgument(seqNumberCL, 1);

	ng_sprintf(number, "%d", ngEPGS->MessageCounter);
	SetArgumentElement(seqNumberCL, 0, 0, number);  //Incrementar
	NewCommandLine(message, seqNumberCL);
	ngEPGS->MessageCounter++;

	char* msgToString;
	MessageToString(message, &msgToString);
	len = ng_strlen(msgToString);

	char *SCN;
	GenerateSCNFromCharArrayBinaryPatterns4Bytes(msgToString, len, &SCN);
	ng_free(msgToString);
	NgCommand* scnCL = ng_create_command("-scn", "--seq", "0.1");
	NewArgument(scnCL, 1);
	SetArgumentElement(scnCL, 0, 0, SCN);
	ng_free(SCN);
	NewCommandLine(message, scnCL);

	ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray(message);

	ng_free(pidSCN);

	*expositionMessage = message;

	return result;
}
