/*
 * epgs_defines.h
 *
 *  Created on: 11/01/2016
 *      Author: vaner
 */

#ifndef EPGS_DEFINES_H_
#define EPGS_DEFINES_H_

// Boolean define
//typedef int bool;
//enum { false, true };//Ja tem nos defines do lora

#define bool	_Bool
#define true	1
#define false	0

// ERROR TYPES
#define NG_PROCESSING 2
#define NG_ERROR 1
#define NG_OK 0

// NG CONSTANTS
#define NG_TYPE_MSB 0x12
#define NG_TYPE_LSB 0x34

#define SVN_SIZE 8

static const char SCN_NG_DOMAIN[] = "Intra_Domain";

#define MILLI_SECONDS_PER_CYCLE 1000 // 1 second per cycle

#endif /* EPGS_DEFINES_H_ */
