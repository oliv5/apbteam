/**************************************************************************//**
  \file zclZll.h

  \brief
    ZigBee Light Link (ZLL) functions interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    17.03.10 A. Taradov - Created.
******************************************************************************/
#ifndef _ZCLZLL_H
#define	_ZCLZLL_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <intrpData.h>
#include <zcl.h>
#include <clusters.h>
#include <zclZllFrameFormat.h>

/******************************************************************************
                        Definitions section
******************************************************************************/
/* Default network parameters, for debug purpose */
#define ZCL_ZLL_DEFAULT_WORKING_CHANNEL    0x0f

/* ZLL Profile Constants */
#define APLC_INTERPAN_TRANS_ID_LIFE_TIME   8000    // aplcInterPANTransIdLifeTime
#define APLC_MAX_PERMIT_JOIN_DURATION      60000   // aplcMaxPermitJoinDuration
#define APLC_MIN_STARTUP_DELAY_TIME        2000    // aplcMinStartupDelayTime
#define APLC_RX_WINDOW_DURATIO             5000    // aplcRxWindowDuration
#define APLC_SCAN_TIMEBASE_DURATION        250     // aplcScanTimeBaseDuration

/******************************************************************************
                        Types section
******************************************************************************/
typedef enum _ZCL_ZllStatus_t
{
  ZCL_ZLL_SUCCESS_STATUS          = 0x00,
  ZCL_ZLL_SCAN_RESULT_STATUS      = 0xf0,
  ZCL_ZLL_SCAN_FINISHED_STATUS    = 0xf1,
  ZCL_ZLL_SCAN_ABORTED_STATUS     = 0xf2,
  ZCL_ZLL_IDENTIFY_START_STATUS   = 0xf3,
  ZCL_ZLL_IDENTIFY_STOP_STATUS    = 0xf4,
  ZCL_ZLL_INVALID_SCENARIO_STATUS = 0xf5,
} ZCL_ZllStatus_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Reset the ZLL layer

This function should be called prior to using any ZLL features. The only
function called before ZLL reset is ZCL_ZllIbSetAppData(), which provides
memory that will be used to store application data.

The function restores ZLL and stack parameters from EEPROM as well as 
applicaiton data.
******************************************************************************/
void ZCL_ZllReset(void);

/**************************************************************************//**
\brief Get next output sequence number
\returns next output sequence number.
******************************************************************************/
uint8_t ZCL_ZllGetSeq(void);

/**************************************************************************//**
\brief Generate new Transaction Identifier and start transaction timer for it
******************************************************************************/
void ZCL_ZllGenerateTransactionId(void);

/**************************************************************************//**
\brief Get current Transaction Identifier
\returns Current Transaction Identifier.
******************************************************************************/
uint32_t ZCL_ZllTransactionId(void);

/**************************************************************************//**
\brief Generate new Response Identifier
******************************************************************************/
void ZCL_ZllGenerateResponseId(void);

/**************************************************************************//**
\brief Set Response Identifier to specified value
\param[in] rid - Response Identifier value to be set
******************************************************************************/
void ZCL_ZllSetResponseId(uint32_t rid);

/**************************************************************************//**
\brief Get current Response Identifier
\returns Current Response Identifier.
******************************************************************************/
uint32_t ZCL_ZllResponseId(void);

/**************************************************************************//**
\brief Get other device's address (after Scan Request has been received)
\returns Other device's extended address.
******************************************************************************/
uint64_t ZCL_ZllOtherDeviceAddress(void);

/**************************************************************************//**
\brief Check if Scan Response has already been received from the device
\returns true if Scan Response has already been received from the device.
******************************************************************************/
bool ZCL_ZllDublicateRejection(ExtAddr_t ieee);

/**************************************************************************//**
\brief Assign a new address from the range of free addresses
\returns Newly assigned address or 0 in case of an error.
******************************************************************************/
ShortAddr_t ZCL_ZllAssignAddress(void);

/**************************************************************************//**
\brief Assign a new group ID from the range of free group IDs
\returns Newly assigned group ID or 0 in case of an error.
******************************************************************************/
uint16_t ZCL_ZllAssignGroupId(void);

/**************************************************************************//**
\brief Check if the device is in the Factory New state

A router in ZLL applications loses its factory new state after it receives the
ZLL start network request during commissioning. An end device becomes not a
factory new device after it commissions the first router, thus creating its own
network, or joins an existing network through another end device (this is called
touch link between end devices).

The application may switch a device back to the factory new state by calling the
ZCL_ZllResetToFactoryNewRequest() function.

\returns Factory New status: \c true if the device is in the Factory New state
******************************************************************************/
bool ZCL_ZllIsFactoryNew(void);

/**************************************************************************//**
\brief Get other device's address (after Scan Request has been received)
\param[in] header - pointer to the header to be filled
\param[in] commandId - command ID to be set in the header
******************************************************************************/
void zclZllFillFrameHeader(ZclZllFrameHeader_t *header, ZclZllCommandId_t commandId);

#endif // _ZCLZLL_H

// eof zclZll.h
