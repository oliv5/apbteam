/**************************************************************************//**
  \file zclZllScan.h

  \brief
    ZLL Scan functions interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    17.03.10 A. Taradov - Created.
******************************************************************************/
#ifndef _ZCLZLLSCAN_H
#define	_ZCLZLLSCAN_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zclZll.h>
#include <zclZllIB.h>
#include <intrpData.h>

/******************************************************************************
                    Definitions section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/

typedef struct _ZCL_ZllScanDeviceInfo_t
{
  // Device Information Table entry
  uint64_t     ieee;
  uint8_t      ep;
  uint16_t     profileId;
  uint16_t     deviceId;
  uint8_t      version;
  uint8_t      groupIds;
  uint8_t      sort;

  // Common fields
  uint8_t      rssiCorrection;
  ZclZllZigBeeInfo_t zigBeeInfo;
  ZclZllInfo_t zllInfo;
  uint16_t     keyBitMask;
  uint32_t     responseId;
  uint64_t     extPanId;
  uint8_t      nwkUpdateId;
  uint8_t      channel;
  uint16_t     panId;
  uint16_t     networkAddress;
  uint8_t      numberSubDevices;
  uint8_t      totalGroupIds;
  uint64_t     ieeeRelayerScanRequest;
  int8_t       rssi;
} ZCL_ZllScanDeviceInfo_t;

typedef struct _ZCL_ZllScanConf_t
{
  ZCL_ZllScanDeviceInfo_t info;
  ZCL_ZllStatus_t         status;
  bool                    stopScan;
} ZCL_ZllScanConf_t;

typedef struct _ZCL_ZllScanReq_t
{
  ZCL_ZllScanConf_t confirm;
  void (*ZCL_ZllScanConf)(ZCL_ZllScanConf_t *conf);
} ZCL_ZllScanReq_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Reset ZLL Scan.
******************************************************************************/
void ZCL_ZllScanReset(void);

/**************************************************************************//**
\brief ZLL Scan request. Should be called only by end devices.

The function is called on end devices to discover other devices by scanning
supported radio channels. Before calling this function the device should be 
prepared to handle Inter-PAN messages: call the ZCL_ZllInterPanStartReq() 
function first.

The callback function specified in the parameters is called once for
each discovered device.

\param req - request parameters
******************************************************************************/
void ZCL_ZllScanReq(ZCL_ZllScanReq_t *req);

/**************************************************************************//**
\brief INTRP-DATA.indication primitive
\param[in] ind - indication parameters
******************************************************************************/
void ZCL_ZllScanReqDataInd(INTRP_DataInd_t *ind);

#endif // _ZCLZLLSCAN_H

// eof zclZllScan.h
