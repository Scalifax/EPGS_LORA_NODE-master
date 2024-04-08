#include "epgs_hello.h"


#include "epgs_wrapper.h"
#include "ng_message.h"
#include "ng_command.h"
#include "ng_epgs_hash.h"

extern char ucPID[];

int ActionRunHello(NgNetInfo *hwInfo, NgMessage **helloMessage) {

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
	ng_create_message(ng_GetTime(), 0, false, &message);


	NgCommand* CL = ng_create_command("-m", "--cl", "0.1");
	NewArgument(CL, 1);
	SetArgumentElement(CL, 0, 0, limiterSCN);
	
	NewArgument(CL, 4);
	SetArgumentElement(CL, 1, 0, hidSCN);
	SetArgumentElement(CL, 1, 1, soidSCN);
	SetArgumentElement(CL, 1, 2, pidSCN);
	SetArgumentElement(CL, 1, 3, "NULL");

	ng_free(limiterSCN);

	NewArgument(CL, 4);
	SetArgumentElement(CL, 2, 0, "empty");
	SetArgumentElement(CL, 2, 1, "empty");
	SetArgumentElement(CL, 2, 2, "empty");
	SetArgumentElement(CL, 2, 3, "empty");
	
	NewCommandLine(message, CL);
	
	NgCommand* helloCL = ng_create_command("-hello", "--ihc", "0.1");
	NewArgument(helloCL, 9);
	SetArgumentElement(helloCL, 0, 0, hidSCN);
	SetArgumentElement(helloCL, 0, 1, soidSCN);
	SetArgumentElement(helloCL, 0, 2, pidSCN);
	SetArgumentElement(helloCL, 0, 3, "NULL");
	SetArgumentElement(helloCL, 0, 4, "NULL"); //
	SetArgumentElement(helloCL, 0, 5, "NULL"); //
	SetArgumentElement(helloCL, 0, 6, hwInfo->Stack);
	SetArgumentElement(helloCL, 0, 7, hwInfo->Interface);
	SetArgumentElement(helloCL, 0, 8, hwInfo->Identifier);
	
	NewCommandLine(message, helloCL);
	
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

	ng_free(hidSCN);
	ng_free(soidSCN);
	ng_free(pidSCN);

	*helloMessage = message;
	
	return NG_OK;
}
