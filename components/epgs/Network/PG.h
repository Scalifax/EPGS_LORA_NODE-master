#ifndef NG_PG_H_
#define NG_PG_H_

#include "epgs_structures.h"
#include "ng_message.h"

//#define DEFAULT_MTU 128 // NovaGenesis was 1200
#define DEFAULT_MTU 222 // NEW
#define DEFAULT_MTU2 222 // NEW
#define HEADER_SIZE_FIELD_SIZE 8
#define HEADER_SEGMENTATION_FIELD_SIZE 8
#define ETHERNET_MAC_ADDR_FIELD_SIZE 6
#define ETHERNET_TYPE_FIELD_SIZE 2

void convertStrToMAC(char* macSTR, char** macBytes);
int getNumberOfMessages(int msgSize);
int getNumberOfMessages2(int msgSize);

int sendNGMessage(NgEPGS* ngEPGS, NgMessage* message, bool isBroadcast);
int newMessageReceived(struct _ng_epgs **ngEPGS, const char* message, int rcvdMsgSize);


#endif /* NG_PG_H_ */
