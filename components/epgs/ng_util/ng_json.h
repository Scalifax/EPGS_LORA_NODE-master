/*
 * ng_json.h
 *
 *  Created on: 10/08/2015
 *      Author: Vaner Jose Magalhaes
 *		e-mail: vaner@inatel.br 
 */

#ifndef _NG_JSON_H
#define _NG_JSON_H

typedef struct _ng_json NgJSon;

extern NgJSon *ng_json_create(const char *json);
extern void ng_json_destroy(NgJSon *ngJSON);
extern void ng_json_add_int(NgJSon *ngJSON, const char* name, int value);
extern void ng_json_add_string(NgJSon *ngJSON, const char* name, const char* value);
extern void ng_json_add_element(NgJSon *ngJSON, const char* element);
extern void ng_json_getJSon(NgJSon *ngJSON, char** jsonStr);
extern void ng_json_get_string(NgJSon *ngJSON, const char* name, char** valueStr);
extern int ng_json_get_int(NgJSon *ngJSON, const char* name);

#endif /* _NG_JSON_H*/
