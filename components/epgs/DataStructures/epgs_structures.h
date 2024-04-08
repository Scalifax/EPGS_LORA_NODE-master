/*
 * epgs_structures.h
 *
 *  Created on: 05/02/2016
 *      Author: vaner
 */

#ifndef DATASTRUCTURES_EPGS_STRUCTURES_H_
#define DATASTRUCTURES_EPGS_STRUCTURES_H_

#include "epgs_defines.h"

enum ng_states {
	HELLO = 1,
	WAIT_HELLO_PGCS,
	EXPOSITION,
	SERVICE_OFFER,
	WAIT_SERVICE_ACCEPTANCE_NOTIFY,
	SUBSCRIBE_SERVICE_ACCEPTANCE,
	WAIT_SERVICE_ACCEPTANCE_DELIVERY,
	PUB_DATA
};
typedef enum ng_states States;

struct _ng_net_info {

	// HID
	char HID[SVN_SIZE + 1];
	// SOID
	char SOID[SVN_SIZE + 1];
	// Stack
	char*	Stack;				// "Ethernet"
	// Interface
	char*	Interface;			// "eth0"
	// Identifier
	char*	Identifier;			// "00:12:34:AB:CD:EF"

};

struct _ng_hw_descriptor {
	int keyWordsCounter;
	char** keyWords;

	int featureCounter;
	char** sensorFeatureName;	// "Accuracy; Resolution"
	char** sensorFeatureValue;	// "0.1; 0.1"

};

struct _ng_pgcs_info {

	// GW_SCN
	char	GW_SCN[SVN_SIZE + 1];
	// HT_SCN
	char	HT_SCN[SVN_SIZE + 1];
	// CORE_BID_SCN
	char	CORE_BID_SCN[SVN_SIZE + 1];
	// Stack
	char*	Stack;				// "wi-fi"
	// Interface
	char*	Interface;			// "eth0"
	// Identifier
	char*	Identifier;			// "00:12:34:AB:CD:EF"

};

struct _ng_scn_id_info {

	// HID
	char	HID[SVN_SIZE + 1];
	// OSID
	char	OSID[SVN_SIZE + 1];
	// PID
	char	PID[SVN_SIZE + 1];
	// BID
	char	BID[SVN_SIZE + 1];

};

struct _ng_received_msg {

	unsigned 	msg_id;

	unsigned long long mgs_size;

	int number_of_frames;

	int frames_read;

	char*	buffer;
};

struct _ng_epgs {
	struct _ng_net_info *NetInfo;
	struct _ng_pgcs_info *PGCSNetInfo;
	struct _ng_scn_id_info *PGCSScnIDInfo;
	struct _ng_scn_id_info *PSSScnIDInfo;
	struct _ng_scn_id_info *APPScnIDInfo;
	struct _ng_hw_descriptor *HwDescriptor;

	struct _ng_received_msg *ReceivedMsg;

	States ngState;
	long HelloCounter;
	int MessageCounter;

	unsigned long long LoopCounter;
	unsigned long long MilliSecondsPerLoop;
	unsigned long long MilliSecondsPerCycle;

	char* key;

	char* pubDataFileName;
	char* pubData;
	unsigned long long pubDataSize;
};


typedef struct _ng_net_info NgNetInfo;
typedef struct _ng_hw_descriptor NgHwDescriptor;
typedef struct _ng_pgcs_info NgPGCSInfo;
typedef struct _ng_scn_id_info NgScnIDInfo;
typedef struct _ng_received_msg NgReceivedMsg;
typedef struct _ng_epgs NgEPGS;

void destroy_NgNetInfo(struct _ng_net_info **ngHWInfo);
void destroy_NgHwDescriptor(struct _ng_hw_descriptor **ngHwDescriptor);
void destroy_NgPGCSInfo(struct _ng_pgcs_info **ngPeerInfo);
void destroy_NgScnIDInfo(struct _ng_scn_id_info **ngScnIDInfo);
void destroy_NgReceivedMsg(struct _ng_received_msg **ngReceivedMsg);
void destroy_NgEPGS(struct _ng_epgs **ngEPGS);

#endif /* DATASTRUCTURES_EPGS_STRUCTURES_H_ */
