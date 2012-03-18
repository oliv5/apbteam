/**************************************************************************//**
  \file zclZllMisc.h

  \brief
    ZLL miscellaneous functionality interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    19.03.10 A. Taradov - Created.
******************************************************************************/
#ifndef _ZCLZLLMISC_H
#define	_ZCLZLLMISC_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zclZll.h>
#include <zclZllIB.h>
#include <intrpData.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define ZCL_ZLL_USE_DEFAULT_IDENTIFY_TIME    0xffff

/******************************************************************************
                    Types section
******************************************************************************/

/******************************************************************************
                    Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Identify notification

The function shall be implemented by the application. The ZLL component
calls this function when an indetify request is received (sent from a remote
device with the ZCL_ZllIdentifyRequest() function).

Receiving this notification, a router is expected to somehow identify itself 
for the user; for example, a lighting device may blink.

\param[in] status - notification status
\param[in] time - identification time
******************************************************************************/
extern void ZCL_ZllIdentifyInd(ZCL_ZllStatus_t status, uint16_t time);

/**************************************************************************//**
\brief Reset ZLL miscellaneous module
******************************************************************************/
void ZCL_ZllMiscReset(void);

/**************************************************************************//**
\brief Send Identify Request command

The function sends an identify request to the destination device specified 
with its extended address. On the destination device ZCL_ZllIdentifyInd() is
called by the stack to indicate identify command reception. The device that
sends the request should first select the destination device, using the 
ZCL_ZllSelectDeviceReq() function.

The function is used during ZLL commissioning via touch link to allow the
application to drop the commissioning procedure if something goes wrong
(for example, the user discovers that wrong device has been selected).

\param[in] addr - destination device extended address pointer
\param[in] identifyTime - time in 1/10ths of a seconds
\returns true if the command has been sent
******************************************************************************/
bool ZCL_ZllIdentifyRequest(ExtAddr_t *addr, uint16_t identifyTime);

/**************************************************************************//**
\brief Reset the device to Factory New state
\param[in] addr - destination short address
\returns true if command was sent.
******************************************************************************/
bool ZCL_ZllResetToFactoryNewRequest(ShortAddr_t addr);

/**************************************************************************//**
\brief INTRP-DATA.indication primitive
\param[in] ind - indication parameters
******************************************************************************/
void ZCL_ZllMiscDataInd(INTRP_DataInd_t *ind);

#endif // _ZCLZLLMISC_H

// eof zclZllMisc.h
