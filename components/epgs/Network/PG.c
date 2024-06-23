#include "PG.h"
#include "epgs_wrapper.h"
#include "ng_util.h"
#include "epgs_controller.h"
#include <stdio.h>
#include <stdlib.h>
#include "runepgs.h"
#include <string.h>

//#include "vcom.h"
//#include "cmsis_os.h"

extern SemaphoreHandle_t xSemaphore;
/**
 * Fun��o para converter uma string MAC (11:22:33:44:55:66 com 17 caracteres) em
 * um vetor de bytes (6 bytes)
 */
void convertStrToMAC(char* macSTR, char** macBytes) {
	
	char *destinationMAC = (char*) ng_malloc(sizeof(char) * 6);
	
	char *end_str;
	char* stringDestMAC = (char*) ng_calloc(sizeof(char), ng_strlen(macSTR)+1);
	ng_strcpy(stringDestMAC, macSTR);
	
	char* token = strtok_r(stringDestMAC, ":", &end_str);			
	long value = ng_strtoul(token, NULL, 16);
	destinationMAC[0] = value;
	
	token = strtok_r(NULL, ":", &end_str);
	value = ng_strtoul(token, NULL, 16);
	destinationMAC[1] = value;
	
	token = strtok_r(NULL, ":", &end_str);
	value = ng_strtoul(token, NULL, 16);
	destinationMAC[2] = value;
	
	token = strtok_r(NULL, ":", &end_str);
	value = ng_strtoul(token, NULL, 16);
	destinationMAC[3] = value;
	
	token = strtok_r(NULL, ":", &end_str);
	value = ng_strtoul(token, NULL, 16);
	destinationMAC[4] = value;
	
	token = strtok_r(NULL, ":", &end_str);
	value = ng_strtoul(token, NULL, 16);
	destinationMAC[5] = value;
	
	ng_free(stringDestMAC);
	//ng_free(token);
	//ng_free(end_str);
	*macBytes = destinationMAC;
}

/**
 * Fun��o de prepara��o e envio da mensagem NovaGenesis via Ethernet ou WiFi
 */
int sendNGMessage(NgEPGS* ngEPGS, NgMessage* message, bool isBroadcast) {


	if(!message || !ngEPGS || !ngEPGS->NetInfo || !ngEPGS->NetInfo->Identifier) {
		return NG_ERROR;
	}
	
	char *destinationMAC;
	if(!isBroadcast) {
		if(ngEPGS->PGCSNetInfo)
		{
			if(ngEPGS->PGCSNetInfo->Identifier) {
				convertStrToMAC(ngEPGS->PGCSNetInfo->Identifier, &destinationMAC);
			} 
			else {
				return NG_ERROR;
			}
		}
		else {
			return NG_ERROR;
		}
				
	} else {
		destinationMAC = (char*) ng_malloc(sizeof(char) * 6);
		destinationMAC[0] = 0xFF;
		destinationMAC[1] = 0xFF;
		destinationMAC[2] = 0xFF;
		destinationMAC[3] = 0xFF;
		destinationMAC[4] = 0xFF;
		destinationMAC[5] = 0xFF;
	}
	char *sourceMAC;
	convertStrToMAC(ngEPGS->NetInfo->Identifier, &sourceMAC);
	
	long long payloadSize = HEADER_SIZE_FIELD_SIZE + message->MessageSize;

	char* sdu = (char*) ng_malloc(sizeof(char) * payloadSize);

	int i = 0;
	for(i = 0; i < HEADER_SIZE_FIELD_SIZE; i++) {
		sdu[i]=(message->MessageSize >> (8*(7-i))) & 0xff;
	}

	for(i = 0; i < message->MessageSize; i++) {
		sdu[HEADER_SIZE_FIELD_SIZE + i] = message->Msg[i];
	}

	int numberOfMsgs = getNumberOfMessages(message->MessageSize);
	unsigned MessageNumber = ng_rand();
	MessageNumber = (MessageNumber << 16) | ng_rand();

	unsigned int SequenceNumber = 0;


//	printf("RX TASK DISABLED\n");
//	vTaskSuspend( task_rx_handle ); //The channel is busy, RX disabled
	int bytesSent = 0;
	TickType_t msg_tick_time = xTaskGetTickCount();
	int j = 0;
	for(j = 0; j < numberOfMsgs; j++) {
		char *mtu = (char*) ng_calloc(sizeof(char), DEFAULT_MTU);

		int index = 0;
		for(i = 0; i < ETHERNET_MAC_ADDR_FIELD_SIZE; i++) {
			mtu[index + i]=destinationMAC[i];
			mtu[index + i + ETHERNET_MAC_ADDR_FIELD_SIZE]=sourceMAC[i];
		}
		index += ETHERNET_MAC_ADDR_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE; // Destination + Source

		mtu[index] = NG_TYPE_MSB;
		mtu[index + 1] = NG_TYPE_LSB;
		index += ETHERNET_TYPE_FIELD_SIZE;

		unsigned char *HeaderSegmentationField = (unsigned char*)ng_malloc(sizeof(unsigned char)*8);
		HeaderSegmentationField[0]=(MessageNumber >> (8*3)) & 0xff;
		HeaderSegmentationField[1]=(MessageNumber >> (8*2)) & 0xff;
		HeaderSegmentationField[2]=(MessageNumber >> (8*1)) & 0xff;
		HeaderSegmentationField[3]=(MessageNumber >> (8*0)) & 0xff;
		HeaderSegmentationField[4]=(SequenceNumber >> (8*3)) & 0xff;
		HeaderSegmentationField[5]=(SequenceNumber >> (8*2)) & 0xff;
		HeaderSegmentationField[6]=(SequenceNumber >> (8*1)) & 0xff;
		HeaderSegmentationField[7]=(SequenceNumber >> (8*0)) & 0xff;

		for(i = 0; i< HEADER_SEGMENTATION_FIELD_SIZE; i++) {
			mtu[index + i]=HeaderSegmentationField[i];
		}
		ng_free(HeaderSegmentationField);
		SequenceNumber++;
		index += HEADER_SEGMENTATION_FIELD_SIZE;

		for(; index < DEFAULT_MTU && bytesSent < payloadSize; index++) {
			mtu[index]=sdu[bytesSent];
			bytesSent++;
		}
//		while(!(TXPermission == FREE || TXPermission == SENDING)) {	//Wait until the channel is free or txed recently
//			vTaskDelay(1);
//		}
		TXPermission = SENDING;
//		printf("TXPermission = NEEDTX\n");
//		TXPermission = NEEDTX;				//Waiting for a permission
//		while(TXPermission != TXGRANTED) {	//TXPermission == TXGRANTED -> Permission granted
//			vTaskDelay(1);
//		}
//		vTaskDelay((200 + (ng_rand()%300))/ portTICK_PERIOD_MS);
		vTaskDelayUntil( &msg_tick_time, 1000/portTICK_PERIOD_MS );
		printf("\n->->->->->->->->->->->->->->->->->->->->->->->->->->->->->->");
		printf("\nNG FRAG SENT / TICK->%lu / SIZE->%u / ID->%08X / SEQ->%u\n", xTaskGetTickCount()*portTICK_PERIOD_MS, index, MessageNumber, SequenceNumber-1);
		if(SequenceNumber-1==0)
		{
			for(int k = 0; k < 30; k++)
			{
				printf("%02X ", mtu[k]  & 0xFF );
			}
		}
		else
		{
			for(int k = 0; k < 22; k++)
			{
				printf("%02X ", mtu[k]  & 0xFF );
			}
		}
		printf("\n->->->->->->->->->->->->->->->->->->->->->->->->->->->->->->");
		ng_LoRaSendData(mtu, index);// NovaGenesis
		ng_free(mtu);
	}
	printf("\n->->->->->->->->->->->->->->->->->->->->->->->->->->->->->->");
	printf("\nNOVAGENESIS MSG SENT:\n");
	for(int contagem = HEADER_SIZE_FIELD_SIZE; contagem<payloadSize; contagem++)
	{
		printf("%c",sdu[contagem]);
	}
	printf("->->->->->->->->->->->->->->->->->->->->->->->->->->->->->->\n");
	TXPermission = FREE;				//Release the channel
	ng_free(sdu);
	ng_free(sourceMAC);
	ng_free(destinationMAC);
	
	return NG_OK;
}

int newMessageReceived(struct _ng_epgs **ngEPGS, const char* message, int rcvdMsgSize) {

//	int k = 0;
//	for(k = 0; k < 30; k++) {
//		ng_printf("%02X ", message[k]  & 0xFF );
////		PRINTF("%02X ", message[k]  & 0xFF );//NEW
//	}

	int ethAddrHeaderSize = ETHERNET_MAC_ADDR_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE;
	
	if(rcvdMsgSize < ethAddrHeaderSize + ETHERNET_TYPE_FIELD_SIZE + HEADER_SEGMENTATION_FIELD_SIZE) {
		return NG_ERROR;
	}	

	// Checking eth type. NG is type 0x1234
	if(!(message[ethAddrHeaderSize] == NG_TYPE_MSB) || !(message[ethAddrHeaderSize + 1] == NG_TYPE_LSB))
	{
		ng_printf("Not a NG type");
//		PRINTF("Not a NG type");//NEW
		return NG_ERROR;
	}

	int index = ethAddrHeaderSize + ETHERNET_TYPE_FIELD_SIZE;
	int i = 0;

	int header_SeqmentationID_Size = HEADER_SEGMENTATION_FIELD_SIZE / 2;
	int header_SeqmentationCounter_Size = HEADER_SEGMENTATION_FIELD_SIZE / 2;

	unsigned msgSeq = 0;
	for(i = 0; i < header_SeqmentationID_Size; i++) {
		msgSeq |= (message[index + i] & 0x000000FF) << (8*(header_SeqmentationID_Size-1-i));
	}
	index += header_SeqmentationID_Size;

	unsigned msgNumber = 0;
	for(i = 0; i < header_SeqmentationCounter_Size; i++) {
		msgNumber |= (message[index + i] & 0x000000FF) << (8*(header_SeqmentationCounter_Size-1-i));
	}
	index += header_SeqmentationCounter_Size;

	bool b = false;

	printf("\n<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-");
	printf("\nNG FRAG RECEIVED / TICK->%lu / SIZE->%u / ID->%08X / SEQ->%u\n", xTaskGetTickCount()*portTICK_PERIOD_MS, rcvdMsgSize, msgSeq, msgNumber);
	if(message[21]==0)
	{
		for(int k = 0; k < 30; k++)
		{
			printf("%02X ", message[k]  & 0xFF );
		}
	}
	else
	{
		for(int k = 0; k < 22; k++)
		{
			printf("%02X ", message[k]  & 0xFF );
		}
	}
	printf("\n<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-\n");

	if (msgNumber == 0)
	{
		if ((*ngEPGS)->ReceivedMsg == NULL)
		{
			int msgSize = 0;
			for(i = 0; i < HEADER_SIZE_FIELD_SIZE; i++) {
				msgSize |= (message[index + i] & 0x000000FF)<< (8*(HEADER_SIZE_FIELD_SIZE-1-i));
			}
			index += HEADER_SIZE_FIELD_SIZE;

			(*ngEPGS)->ReceivedMsg = (NgReceivedMsg*) ng_malloc(sizeof(NgReceivedMsg)*1);
			(*ngEPGS)->ReceivedMsg->msg_id = msgSeq;
			(*ngEPGS)->ReceivedMsg->frames_read = 0;
			(*ngEPGS)->ReceivedMsg->mgs_size = msgSize;
			(*ngEPGS)->ReceivedMsg->number_of_frames = getNumberOfMessages2(msgSize + HEADER_SIZE_FIELD_SIZE);
			(*ngEPGS)->ReceivedMsg->buffer = (char*) ng_malloc(sizeof(char) * msgSize);
		}
		else if((*ngEPGS)->ReceivedMsg->msg_id != msgSeq)
		{
			ng_printf("\nDif id msg\n");
			ng_free((*ngEPGS)->ReceivedMsg->buffer);
			ng_free((*ngEPGS)->ReceivedMsg);
			(*ngEPGS)->ReceivedMsg->buffer = NULL;
			(*ngEPGS)->ReceivedMsg = NULL;
			return newMessageReceived (ngEPGS, message, rcvdMsgSize);
		}
	}
	else
	{
		if ((*ngEPGS)->ReceivedMsg == NULL)
		{
			return NG_ERROR;
		}
		else
		{
			b=false;
		}
	}

//	if((*ngEPGS)->ReceivedMsg == NULL && msgNumber == 0) {
//		ng_printf("\n1st msg\n");
//		TXPermission = RECEIVING;				//RX Starting, the channel is busy
//		(*ngEPGS)->ReceivedMsg = (NgReceivedMsg*) ng_malloc(sizeof(NgReceivedMsg)*1);
//		(*ngEPGS)->ReceivedMsg->msg_id = msgSeq;
//		(*ngEPGS)->ReceivedMsg->frames_read = 0;
//
//		int msgSize = 0;
//		for(i = 0; i < HEADER_SIZE_FIELD_SIZE; i++) {
//			msgSize |= (message[index + i] & 0x000000FF)<< (8*(HEADER_SIZE_FIELD_SIZE-1-i));
//		}
//		index += HEADER_SIZE_FIELD_SIZE;
//
//		(*ngEPGS)->ReceivedMsg->mgs_size = msgSize;
//		(*ngEPGS)->ReceivedMsg->number_of_frames = getNumberOfMessages2(msgSize + HEADER_SIZE_FIELD_SIZE);
//		(*ngEPGS)->ReceivedMsg->buffer = (char*) ng_malloc(sizeof(char) * msgSize);
//	} else if((*ngEPGS)->ReceivedMsg->msg_id != msgSeq) {
//		ng_printf("\nDif id msg\n");
//		if ((*ngEPGS)->ReceivedMsg->buffer != NULL) {
//			ng_free((*ngEPGS)->ReceivedMsg->buffer);
//			(*ngEPGS)->ReceivedMsg->buffer = NULL;
//		}
//		if (((*ngEPGS)->ReceivedMsg) != NULL) {
//			ng_free((*ngEPGS)->ReceivedMsg);
//			((*ngEPGS)->ReceivedMsg) = NULL;
//		}
//		if(msgNumber == 0) {
//			return newMessageReceived (ngEPGS, message, rcvdMsgSize);
//		}
//		else
//			TXPermission = FREE;				//RX Ending, the channel is free
//		return NG_ERROR;
//	} else {
//		b=false;
//	}

	int bufferIndex = 0;
	if(msgNumber == 0) {
		bufferIndex = 0;
	} else {
		bufferIndex = DEFAULT_MTU2 - (HEADER_SEGMENTATION_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE +  ETHERNET_TYPE_FIELD_SIZE + HEADER_SIZE_FIELD_SIZE) +
				(DEFAULT_MTU2 - (HEADER_SEGMENTATION_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE +  ETHERNET_TYPE_FIELD_SIZE)) * (msgNumber - 1);
	}


	for(i=0; index < rcvdMsgSize; i++, index++) {
		(*ngEPGS)->ReceivedMsg->buffer[bufferIndex + i] = message[index];
	}


	(*ngEPGS)->ReceivedMsg->frames_read++;


	if((*ngEPGS)->ReceivedMsg->frames_read == (*ngEPGS)->ReceivedMsg->number_of_frames && !b) {
		ParseReceivedMessage(ngEPGS);
//		TXPermission = FREE;				//RX Ending, the channel is free
		return NG_OK;
	}

	return NG_PROCESSING;
}

int getNumberOfMessages(int msgSize) {
	int headerSize = HEADER_SEGMENTATION_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE +  ETHERNET_TYPE_FIELD_SIZE;
	int payloadMTU = DEFAULT_MTU - headerSize;

	int payloadSize = HEADER_SIZE_FIELD_SIZE + msgSize;

	return ((int)(payloadSize/ payloadMTU)) + 1;
}

int getNumberOfMessages2(int msgSize) {
	int headerSize = HEADER_SEGMENTATION_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE +  ETHERNET_TYPE_FIELD_SIZE;
	int payloadMTU = DEFAULT_MTU2 - headerSize;

	int payloadSize = HEADER_SIZE_FIELD_SIZE + msgSize;

	return ((int)(payloadSize/ payloadMTU)) + 1;
}