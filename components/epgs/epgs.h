#ifndef NG_EPGS_H_
#define NG_EPGS_H_

#include "epgs_structures.h"

/**
 *
 * Inits the EPGS data structure, flags and counters.
 *
 * @param ngEPGS Reference to the EPGS data structure pointer.
 *
 * @return OK if success, ERROR otherwise (@see epgs_defines.h)
 */
int initEPGS(NgEPGS **ngEPGS);

/**
 * Sets the Hardware configuration
 *
 * @param ngEPGS 		Reference to the EPGS data structure pointer.
 * @param hid 			The Hardware Id - Unique number that describes the hardware
 * @param soid			System Operation Id
 * @param pid			Process Id
 * @param Stack			Type of network stack used (eg: wi-fi, ethernet, bluetooth, ip-v6...)
 * @param Interface		Interface of the network (eg: eth0, em0...)
 * @param Identifier	The identifier of the network (eg: 00:12:34:56:78:9A)
 *
 * @return OK if success, ERROR otherwise (@see epgs_defines.h)
 */
int setHwConfigurations(NgEPGS **ngEPGS, const char* hid, const char* soid, const char* Stack, const char* Interface, const char* Identifier);

/**
 * Adds keywords that specifies the Node
 *
 * @param ngEPGS 		Reference to the EPGS data structure pointer.
 * @param name			The keyword (eg: "Temperature").
 *
 * @return OK if success, ERROR otherwise (@see epgs_defines.h)
 */
int addKeyWords(NgEPGS **ngEPGS, const char* name);

/**
 * Adds sensor features in name/value way.
 *
 * @param ngEPGS 		Reference to the EPGS data structure pointer.
 * @param name			The name of the feature (eg: Temperature_Max).
 * @param value			The value of the feature (eg: 100).
 *
 * @return OK if success, ERROR otherwise (@see epgs_defines.h)
 */
int addHwSensorFeature(NgEPGS **ngEPGS, const char* name, const char* value);

/**
 * Analysis the EPGS state and takes actions. This function must be called in a loop.
 *
 * @param ngEPGS 		Reference to the EPGS data structure pointer.
 *
 * @return State Machine ID (@see epgs_structures.h)
 */
int processLoop(NgEPGS **ngEPGS);

/**
 * Parses the received message and takes actions.
 *
 * @param ngEPGS 		Reference to the EPGS data structure pointer.
 * @param message		The received message.
 * @param msgSize		The size of the received message.
 *
 * @return OK if success, ERROR otherwise (@see epgs_defines.h)
 */
int newReceivedMessage(NgEPGS **ngEPGS, const char *message, int msgSize);

/**
 * Sets the data to be publish in the next cycle.
 *
 * @param ngEPGS 		Reference to the EPGS data structure pointer.
 * @param fileName		The name of file to be published.
 * @param data			Data content of the file to be published.
 * @param dataLengh		Size of the data to be published.
 *
 * @return OK if success, ERROR otherwise (@see epgs_defines.h)
 */
int setDataToPub(NgEPGS **ngEPGS, const char* fileName, const char* data, int dataLength);


#endif /* NG_EPGS_H_ */
