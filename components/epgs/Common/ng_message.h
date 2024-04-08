/*
 * ng_message.h
 *
 *  Created on: 28/09/2015
 *      Author: Vaner Jose Magalhaes
 *		e-mail: vaner@inatel.br 
 */

#ifndef MESSAGE_NG_MESSAGE_H_
#define MESSAGE_NG_MESSAGE_H_

//int Message::ConvertMessageFromCommandLinesandPayloadFileToCharArray()
//int Message::ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()

#include "ng_command.h"
#include "epgs_defines.h"

#define DELETED_BY_APP true
#define DELETED_BY_CORE false

struct _ng_message {
	// Type
	short					Type;

	// CommandLine container
	NgCommand				**CommandLines;

	// Number of command lines
	unsigned int			NoCL;

	// Has payload control flag. True means that the message has a payload.
	bool					HasPayloadFlag;

	// Payload char array. Can be used to carry a payload in memory instead of saving it to a file
	char 					*Payload;

	// Message char array. Can be used to carry the message to memory instead of using a file
	char 					*Msg;

	// The size of the payload in bytes
	unsigned long long				PayloadSize;

	// The size of the message in bytes
	unsigned long long				MessageSize;

	// Time for priority queue comparison
	double					Time;
};

typedef struct _ng_message NgMessage;

void ng_create_message(double _Time, short _Type, bool _HasPayload, NgMessage ** ngMessage);

void ng_destroy_message(struct _ng_message **ngMessage);

int NewCommandLine(struct _ng_message *ngMessage, NgCommand *CL);

// Get a CommandLine by its index
int GetCommandLine(struct _ng_message *ngMessage, unsigned int _Index, NgCommand **CL);

// Set *Payload from char array. A copy of the char array is done. If a previous array was being used, it will be deleted.
int SetPayloadFromCharArray(struct _ng_message *ngMessage, char* _Value, unsigned long long _Size);

// Set *Msg from char array. A copy of the char array is done. If a previous array was being used, it will be deleted.
int SetMessageFromCharArray(struct _ng_message *ngMessage, char* _Value, unsigned long long _Size);

int ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray(struct _ng_message *ngMessage);

void MessageToString(struct _ng_message *ngMessage, char** out);

void MessageFromString(const char* in, unsigned long long msgSize, struct _ng_message **ngMessage);

#endif /* MESSAGE_NG_MESSAGE_H_ */
