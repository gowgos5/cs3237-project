#ifndef _LOGGING_H_
#define _LOGGING_H_

#include "mbed.h"

/**
* @brief This will read the mission location and if there is something valid,
* then run the  Logging_ProcessMissionCmds()
* @param cmdBuffer buffer
*/
uint32_t Logging_IsMissionDefined(uint8_t *cmdBuffer);

int8_t Logging_ReadMissionFromFlash(uint8_t *buffer);

// return the page where mission is defined, Mission specific
uint32_t Logging_GetMissionStartPage(void);

// return the page where the mission definition ends, Mission specific
uint32_t Logging_GetMissionEndPage(void);

// Returns the location where the Writing can start from, for data logging...
uint32_t Logging_GetLoggingStartPage(void);

// Returns the end location available where the Flash ends essentially.... for
// data logging.
uint32_t Logging_GetLoggingEndPage(void);

// returns one if the usb is connected, zero if not
uint32_t Usb_IsConnected(void);

void Logging_SetStart(bool state);

bool Logging_GetStart(void);

#endif /* _LOGGING_H_ */
