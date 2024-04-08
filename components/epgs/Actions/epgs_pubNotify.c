#include "epgs_pubNotify.h"


#include "epgs_wrapper.h"
#include "ng_message.h"
#include "ng_command.h"
#include "ng_epgs_hash.h"



extern char ucPID[];
int actionPublicationAndNotification(NgEPGS *ngEPGS, bool isData, char* fileName, char* filePayload, unsigned long long payloadSize, NgMessage **pubNotifyMessage) {
	
	if(!ngEPGS || !ngEPGS->NetInfo || !ngEPGS->NetInfo->HID || !ngEPGS->NetInfo->SOID)
	{
		return NG_ERROR;
	}
	if(!ngEPGS->PSSScnIDInfo || !ngEPGS->PSSScnIDInfo->HID || !ngEPGS->PSSScnIDInfo->OSID || !ngEPGS->PSSScnIDInfo->PID || !ngEPGS->PSSScnIDInfo->BID)
	{
		return NG_ERROR;
	}	
	if(isData) 
	{
		if(!ngEPGS->APPScnIDInfo || !ngEPGS->APPScnIDInfo->HID || !ngEPGS->APPScnIDInfo->OSID || !ngEPGS->APPScnIDInfo->PID || !ngEPGS->APPScnIDInfo->BID) 
		{
			return NG_ERROR;
		}
	} 
	else {
		if(!ngEPGS->PGCSScnIDInfo || !ngEPGS->PGCSScnIDInfo->HID || !ngEPGS->PGCSScnIDInfo->OSID || !ngEPGS->PGCSScnIDInfo->PID || !ngEPGS->PGCSNetInfo || !ngEPGS->PGCSNetInfo->CORE_BID_SCN)
		{
			return NG_ERROR;
		}
	}	
	
	char* payloadHash;
	unsigned long long len = payloadSize;
	GenerateSCNFromCharArrayBinaryPatterns4Bytes(filePayload, len, &payloadHash);

	len = ng_strlen(ngEPGS->NetInfo->HID);
	char* hidSCN;
	GenerateSCNFromCharArrayBinaryPatterns4Bytes(ngEPGS->NetInfo->HID, len, &hidSCN);

	len = ng_strlen(ngEPGS->NetInfo->SOID);
	char* soidSCN;
	GenerateSCNFromCharArrayBinaryPatterns4Bytes(ngEPGS->NetInfo->SOID, len, &soidSCN);

	len = 4;
	char* pidSCN;
	GenerateSCNFromCharArrayBinaryPatterns4Bytes("4321", len, &pidSCN);
//	GenerateSCNFromCharArrayBinaryPatterns4Bytes(ucEPGS, len, &pidSCN);

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
	ng_free(pidSCN);
	ng_free(limiterSCN);

	NewArgument(CL, 4);
	SetArgumentElement(CL, 2, 0, ngEPGS->PSSScnIDInfo->HID);
	SetArgumentElement(CL, 2, 1, ngEPGS->PSSScnIDInfo->OSID);
	SetArgumentElement(CL, 2, 2, ngEPGS->PSSScnIDInfo->PID);
	SetArgumentElement(CL, 2, 3, ngEPGS->PSSScnIDInfo->BID);
	
	NewCommandLine(message, CL);
	
	NgCommand* notificationCL = ng_create_command("-p", "--notify", "0.1");
	NewArgument(notificationCL, 1);
	SetArgumentElement(notificationCL, 0, 0, "18"); // Category
	
	NewArgument(notificationCL, 1);
	SetArgumentElement(notificationCL, 1, 0, payloadHash); // Key - Hash do arquivo.
	ng_free(payloadHash);
	
	NewArgument(notificationCL, 1);
	SetArgumentElement(notificationCL, 2, 0, fileName); // _Publisher - File Name
	
	NewArgument(notificationCL, 5);
	SetArgumentElement(notificationCL, 3, 0, "pub"); 
	
	if(isData) 
	{
		SetArgumentElement(notificationCL, 3, 1, ngEPGS->APPScnIDInfo->HID);
		SetArgumentElement(notificationCL, 3, 2, ngEPGS->APPScnIDInfo->OSID);
		SetArgumentElement(notificationCL, 3, 3, ngEPGS->APPScnIDInfo->PID);
		SetArgumentElement(notificationCL, 3, 4, ngEPGS->APPScnIDInfo->BID);
	} 
	else 
	{
		SetArgumentElement(notificationCL, 3, 1, ngEPGS->PGCSScnIDInfo->HID);
		SetArgumentElement(notificationCL, 3, 2, ngEPGS->PGCSScnIDInfo->OSID);
		SetArgumentElement(notificationCL, 3, 3, ngEPGS->PGCSScnIDInfo->PID);
		SetArgumentElement(notificationCL, 3, 4, ngEPGS->PGCSNetInfo->CORE_BID_SCN);
	}
	
	NewCommandLine(message, notificationCL);
	
	NgCommand* infoCL = ng_create_command("-info", "--payload", "0.1");
	NewArgument(infoCL, 1);
	SetArgumentElement(infoCL, 0, 0, fileName); 
	NewCommandLine(message, infoCL);
	
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
	
	SetPayloadFromCharArray(message, filePayload, (long long) payloadSize);
	
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
		
	*pubNotifyMessage = message;
	
	return NG_OK;
}
