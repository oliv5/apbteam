/**************************************************************************//**
  \file zclZllIB.h

  \brief
    ZLL Information Base interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    18.03.10 A. Taradov - Created.
******************************************************************************/
#ifndef _ZCLZLLIB_H
#define	_ZCLZLLIB_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <types.h>
#include <aps.h>
#include <zclZllFrameFormat.h>
#include <zclZllIB.h>

/******************************************************************************
                        Definitions section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/

// Device Information Table entry
typedef struct _DitEntry_t
{
  uint64_t ieee;
  uint8_t  ep;
  uint16_t profileId;
  uint16_t deviceId;
  uint8_t  version;
  uint8_t  groupIds;
  uint8_t  sort;
} ZCL_ZllDitEntry_t;

typedef struct _ZCL_ZllDevice_t
{
  uint8_t     factoryNew;
  uint8_t     channel;
  PanId_t     panId;
  ExtPanId_t  extPanId;
  ShortAddr_t nwkAddr;
  ExtAddr_t   extAddr;
  ShortAddr_t freeNwkAddressRangeBegin;
  ShortAddr_t freeNwkAddressRangeEnd;
  uint16_t    freeGroupIdRangeBegin;
  uint16_t    freeGroupIdRangeEnd;
  uint16_t    groupIdsBegin;
  uint16_t    groupIdsEnd;
  uint8_t     nwkKey[SECURITY_KEY_SIZE];
} ZCL_ZllDevice_t;

typedef struct _ZCL_ZllIb_t
{
  ZCL_ZllDevice_t     device;
  ZclZllZigBeeInfo_t  zigBeeInfo;
  ZclZllInfo_t        zllInfo;
  ZCL_ZllDitEntry_t   *dit;
  uint8_t             ditSize;
  uint8_t             numberSubDevices;
  uint8_t             totalGroupIds;
  MAC_CapabilityInf_t capabilityInf;
  uint8_t             *appData;
  uint16_t            appDataSize;
} ZCL_ZllIb_t;

/******************************************************************************
                        Global variables
******************************************************************************/
extern ZCL_ZllIb_t zclZllIB;

/******************************************************************************
                        Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Initialize Information Base and restore ZLL parameters and
application data

This function is invoked by the ZCL_ZllReset() function and therefore should
not be called by the user directly.

The function restores ZLL parameters and application data from EEPROM and sets
the restored network and stack parameters in Configuration Server as well.
******************************************************************************/
void ZCL_ZllIbReset(void);

/**************************************************************************//**
\brief Set application data that should be saved and restored

This function specifies the memory which is used to store and restore 
application data. The data will be saved in EEPROM when the ZCL_ZllIbSave() 
function is called, and restored when ZCL_ZllIbReset() or ZCL_ZllReset() 
functions are called (ZCL_ZllReset() invokes ZCL_ZllIbReset()).

This function should be called before ZCL_ZllIbReset() or ZCL_ZllReset() to
specify the memory to which application data will be restored.

Application data may be any abstract data. However, keeping network and
stack parameters in application data is unnecunnecessary, because they are
maintained by ZLL.

\param[in] data - application data
\param[in] button - button number
******************************************************************************/
void ZCL_ZllIbSetAppData(uint8_t *data, uint16_t size);

/**************************************************************************//**
\brief Save ZLL parameters and application data in EEPROM

The functions saves ZLL parameters and application data in EEPROM so that
they can be restored after device is reset. ZLL parameters are stored inside
the stack in the instance of the ZCL_ZllDevice_t type. This function should
be called each time after the application data is modofied.
******************************************************************************/
void ZCL_ZllIbSave(void);

/**************************************************************************//**
\brief Reset ZLL parameters to factory new settings and store them into EEPROM
******************************************************************************/
void ZCL_ZllIbResetToFactoryNew(void);

/**************************************************************************//**
\brief Set Device Information Table

Device Information Table contains entries desribing logical device type 
implemented on the device. However, most frequently the table has only one entry.
Since the table stores static information about application functionality, it is
not writtern into EEPROM, but is typically configured each time the application
starts.

The following example shows how to fill a table with a single entry:

\code
\\Global definition of the Device Information table
ZCL_ZllDitEntry_t deviceInfoTable[DEVICE_INFO_TABLE_ENTRIES];
...
CS_ReadParameter(CS_UID_ID, &deviceInfoTable[0].ieee); //Read extended address from the Configuration Server
deviceInfoTable[0].ep        = APP_ENDPOINT_COLOR_SCENE_REMOTE;
deviceInfoTable[0].profileId = APP_PROFILE_ID;
deviceInfoTable[0].deviceId  = APP_DEVICE_ID;
deviceInfoTable[0].version   = APP_VERSION;
deviceInfoTable[0].groupIds  = APP_GROUP_IDS; //The number of group IDs
deviceInfoTable[0].sort      = 0;
ZCL_ZllIbSetDeviceInfoTable(deviceInfoTable, 1);
\endcode

Note that all constants starting with APP_ should be defined by the
application.

\param[in] dit - pointer to the Device Information table
\param[in] size - number of entries in the table
******************************************************************************/
void ZCL_ZllIbSetDeviceInfoTable(ZCL_ZllDitEntry_t dit[], uint8_t size);

#endif // _ZCLZLLIB_H

// eof zclZllIB.h
