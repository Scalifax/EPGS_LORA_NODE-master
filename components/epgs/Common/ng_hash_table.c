/*
 * ng_hash_table.c
 *
 *  Created on: 21/12/2016
 *      Author: Vaner José Magalhães
 *		E-mail: vaner@inatel.br
 */

#include "ng_hash_table.h"
#include "../epgs_wrapper.h"
#include "../ng_util/ng_util.h"
#include "../epgs_defines.h"

void ng_hash_table_create(int category, NgHashTable** ngHashTable) {
	NgHashTable *p_hashTable = (NgHashTable*)ng_malloc(sizeof(NgHashTable) * 1);

	p_hashTable->entries = NULL;
	p_hashTable->category = category;
	p_hashTable->entriesCount = 0;

	*ngHashTable = p_hashTable;

}

int ng_hash_table_get_position(NgHashTable *ngHashTable, char* Name) {

	int i = 0;
	for(i = 0; i < ngHashTable->entriesCount; i++) {
		if(ng_strcmp(ngHashTable->entries[i]->Name, Name) == 0) {
			return i;
		}
	}
	return -1;
}

int ng_hash_table_put(NgHashTable *ngHashTable, char* Name, int nameSize, char* Value, int valueSize) {
	int position = ng_hash_table_get_position(ngHashTable, Name);

	if(position == -1) {
		ngHashTable->entries = (NgHashEntry**) ng_realloc(ngHashTable->entries, (ngHashTable->entriesCount + 1) * sizeof(NgHashEntry*));

		ngHashTable->entries[ngHashTable->entriesCount] = (NgHashEntry*) ng_malloc(sizeof(NgHashEntry)*1);

		ngHashTable->entries[ngHashTable->entriesCount]->nameSize = nameSize;
		ngHashTable->entries[ngHashTable->entriesCount]->Name = (char*)ng_malloc(sizeof(char) * (nameSize+1));
		ng_strcpy(ngHashTable->entries[ngHashTable->entriesCount]->Name, Name);

		ngHashTable->entries[ngHashTable->entriesCount]->valueSize = valueSize;
		ngHashTable->entries[ngHashTable->entriesCount]->Value = (char*)ng_malloc(sizeof(char) * (valueSize+1));
		ng_strcpy(ngHashTable->entries[ngHashTable->entriesCount]->Value, Value);

		ngHashTable->entriesCount++;
	} else {
		ng_free(ngHashTable->entries[position]->Value);
		ngHashTable->entries[position]->Value = NULL;

		ngHashTable->entries[position]->valueSize = valueSize;
		ngHashTable->entries[position]->Value = (char*)ng_malloc(sizeof(char) * (valueSize+1));
		ng_strcpy(ngHashTable->entries[position]->Value, Value);
	}

	return NG_OK;

}

int ng_hash_table_get(NgHashTable *ngHashTable, char* Name, char** Value) {
	int position = ng_hash_table_get_position(ngHashTable, Name);

	if(position == -1) {
		return NG_ERROR;
	}

	*Value = (char*)ng_malloc(sizeof(char) * (ngHashTable->entries[position]->valueSize+1));
	ng_strcpy(*Value, ngHashTable->entries[position]->Value);

	return NG_OK;
}
