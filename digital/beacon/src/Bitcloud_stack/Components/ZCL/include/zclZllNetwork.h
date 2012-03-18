/**************************************************************************//**
  \file zclZllNetwork.h

  \brief
    ZLL network functionality interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    22.03.10 A. Taradov - Created.
******************************************************************************/
#ifndef _ZCLZLLNETWORK_H
#define	_ZCLZLLNETWORK_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zclZll.h>
#include <zclZllIB.h>
#include <zclZllScan.h>
#include <intrpData.h>

/******************************************************************************
                    Definitions section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct _ZCL_ZllStartNetworkConf_t
{
  ZCL_ZllStatus_t status;
} ZCL_ZllStartNetworkConf_t;

typedef struct _ZCL_ZllStartNetworkReq_t
{
  ZCL_ZllStartNetworkConf_t confirm;
  ZCL_ZllScanDeviceInfo_t   *otherDevice;
  void (*ZCL_ZllStartNetworkConf)(ZCL_ZllStartNetworkConf_t *conf);
} ZCL_ZllStartNetworkReq_t;

/*! Select ZLL device confirm parameters */
typedef struct _ZCL_SelectDeviceConf_t
{
  /* Operation status */
  ZCL_ZllStatus_t status;
} ZCL_SelectDeviceConf_t;

/*! Select ZLL device request parameters */
typedef struct _ZCL_ZllSelectDeviceReq_t
{
  /*! Selected device information */
  ZCL_ZllScanDeviceInfo_t *deviceInfo;
  /*! MAC-SET.request primitive */
  MAC_SetReq_t macSetReq;
  /*! Confirm callback pointer */
  void (*ZCL_ZllSelectDeviceConf)(ZCL_SelectDeviceConf_t *conf);
  /*! Select ZLL device confirm info */
  ZCL_SelectDeviceConf_t confirm;
} ZCL_ZllSelectDeviceReq_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Reset ZLL network module.
******************************************************************************/
void ZCL_ZllNetworkReset(void);

/**************************************************************************//**
\brief Send ZLL start network request. Should be called only by end devices.

The function sends an Inter-PAN command containing network parameters to 
the selected router. The router is notified about receiving network parameters
via ZCL_ZllStartInd() (this function should be implemented on the router). 
Upon receiving such indication the router is expected to rejoin the nework,
using ZDO functions. The end device that has issued the request should also
rejoin the network to join the router.

The device to which the ZLL start network request will be sent is set
with the ZCL_ZllSelectDeviceReq() function.

\param req - request parameters
******************************************************************************/
void ZCL_ZllStartNetworkReq(ZCL_ZllStartNetworkReq_t *req);

/**************************************************************************//**
\brief INTRP-DATA.indication primitive
\param[in] ind - indication parameters
******************************************************************************/
void ZCL_ZllNetworkDataInd(INTRP_DataInd_t *ind);

/**************************************************************************//**
\brief Select a device discovered during scanning

Using this function, the application specifies a device to which it will
send an identify request and a start network request. Information about the
device that should be provided for this function is received during network 
scan (performed by ZCL_ZllScanReq()).

\param[in] req - request parameters
******************************************************************************/
void ZCL_ZllSelectDeviceReq(ZCL_ZllSelectDeviceReq_t *req);

#endif // _ZCLZLLNETWORK_H

// eof zclZllNetwork.h
