/*
 * ng_command.h
 *
 *  Created on: 21/10/2015
 *      Author: vaner
 */

#ifndef COMMAND_NG_COMMAND_H_
#define COMMAND_NG_COMMAND_H_

#include "epgs_defines.h"

struct _ng_arguments {
	char** Elements;
	unsigned int NoE;
};

struct _ng_command {
	// Command Name
	char*	Name;  // ng -msg

	// Command Alternative
	char*	Alternative; // --cl

	// Command Version
	char*	Version; // 0.1

	// Arguments and elements container
	struct _ng_arguments **Arguments;

	// Number of arguments
	unsigned int	NoA;



};

typedef struct _ng_command NgCommand;
typedef struct _ng_arguments NgArguments;


NgCommand *ng_create_command(char* _Name, char* _Alternative, char* _Version);

void ng_destroy_command(struct _ng_command **ngCommand);

// Allocates and add a string vector on Arguments container
void NewArgument(struct _ng_command *ngCommand, int _Size);

// Get argument
int GetArgument(struct _ng_command *ngCommand, unsigned int _Index, NgArguments** _Argument);

// Get number of arguments
int GetNumberofArguments(struct _ng_command *ngCommand, unsigned int *_Number);

// Get number of elements in a certain argument
int GetNumberofArgumentElements(struct _ng_command *ngCommand, unsigned int _Index, unsigned int *_Number);

// Set an element at an Argument
int SetArgumentElement(struct _ng_command *ngCommand, unsigned int _Index, unsigned int _Element, char* _Value);

// Get an element at an Argument
int GetArgumentElement(struct _ng_command *ngCommand, unsigned int _Index, unsigned int _Element, char** _Value);

char* ConvertNgCommandToString(struct _ng_command *CL);

NgCommand *ConvertStringToNgCommand(char *inputStr);

#endif /* COMMAND_NG_COMMAND_H_ */
