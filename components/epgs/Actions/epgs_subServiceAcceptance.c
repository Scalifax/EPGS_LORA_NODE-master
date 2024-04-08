#include "epgs_subServiceAcceptance.h"
#include "epgs_wrapper.h"
#include "ng_message.h"
#include "ng_command.h"
#include "ng_epgs_hash.h"

extern char ucPID[];

int ActionSubscriptionServiceAcceptance(NgNetInfo *hwInfo, NgScnIDInfo *pgcsSCNInfo, NgScnIDInfo *pssSCNInfo, int seqNumber, char* key, NgMessage **subServiceAcceptanceMessage) {


	long long len = ng_strlen(hwInfo->HID);
	char* hidSCN;
	GenerateSCNFromCharArrayBinaryPatterns4Bytes(hwInfo->HID, len, &hidSCN);

	len = ng_strlen(hwInfo->SOID);
	char* soidSCN;
	GenerateSCNFromCharArrayBinaryPatterns4Bytes(hwInfo->SOID, len, &soidSCN);
	
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
	ng_free(pidSCN);
	ng_free(limiterSCN);

	NewArgument(CL, 4);
	SetArgumentElement(CL, 2, 0, pssSCNInfo->HID);
	SetArgumentElement(CL, 2, 1, pssSCNInfo->OSID);
	SetArgumentElement(CL, 2, 2, pssSCNInfo->PID);
	SetArgumentElement(CL, 2, 3, pssSCNInfo->BID);
	
	NewCommandLine(message, CL);
	
	NgCommand* bindCat18 = ng_create_command("-s", "--b", "0.1");
	NewArgument(bindCat18, 1);
	SetArgumentElement(bindCat18, 0, 0, "18"); // Category	
	NewArgument(bindCat18, 1);
	SetArgumentElement(bindCat18, 1, 0, key); // Key - Hash do arquivo.	
	NewCommandLine(message, bindCat18);
		
	NgCommand* bindCat2 = ng_create_command("-s", "--b", "0.1");
	NewArgument(bindCat2, 1);
	SetArgumentElement(bindCat2, 0, 0, "2"); // Category	
	NewArgument(bindCat2, 1);
	SetArgumentElement(bindCat2, 1, 0, key); // Key - Hash do arquivo.	
	NewCommandLine(message, bindCat2);
	
	NgCommand* bindCat9 = ng_create_command("-s", "--b", "0.1");
	NewArgument(bindCat9, 1);
	SetArgumentElement(bindCat9, 0, 0, "9"); // Category	
	NewArgument(bindCat9, 1);
	SetArgumentElement(bindCat9, 1, 0, key); // Key - Hash do arquivo.	
	NewCommandLine(message, bindCat9);
	
	char number[4];
	NgCommand* msgTypeCL = ng_create_command("-message", "--type", "0.1");
	NewArgument(msgTypeCL, 1);
	ng_sprintf(number, "%d", message->Type);
	SetArgumentElement(msgTypeCL, 0, 0, number); 
	NewCommandLine(message, msgTypeCL);
	
	NgCommand* seqNumberCL = ng_create_command("-message", "--seq", "0.1");
	NewArgument(seqNumberCL, 1);
	
	ng_sprintf(number, "%d", seqNumber);
	SetArgumentElement(seqNumberCL, 0, 0, number);  //Incrementar
	NewCommandLine(message, seqNumberCL);
	
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
		
	*subServiceAcceptanceMessage = message;
	
	return NG_OK;
}
