/*
 * ng_hash_table.h
 *
 *  Created on: 21/12/2016
 *      Author: Vaner José Magalhães
 *		E-mail: vaner@inatel.br
 */

#ifndef COMMON_NG_HASH_TABLE_H_
#define COMMON_NG_HASH_TABLE_H_

struct _ng_hash_entry {
	// Command Name
	char*	Name;  // ng -msg
	int nameSize;

	// Command Alternative
	char*	Value; // --cl
	int valueSize;
};

typedef struct _ng_hash_entry NgHashEntry;

struct _ng_hash_table {
	struct _ng_hash_entry **entries;
	int category;
	int entriesCount;
};

typedef struct _ng_hash_entry NgHashEntry;
typedef struct _ng_hash_table NgHashTable;

void ng_hash_table_create(int category, NgHashTable** ngHashTable);

int ng_hash_table_get_position(NgHashTable *ngHashTable, char* Name);

int ng_hash_table_put(NgHashTable *ngHashTable, char* Name, int nameSize, char* Value, int valueSize);

int ng_hash_table_get(NgHashTable *ngHashTable, char* Name, char** Value);

#endif /* COMMON_NG_HASH_TABLE_H_ */
