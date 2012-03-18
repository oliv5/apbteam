/**************************************************************************//**
  \file zclZllInterPan.h

  \brief
    ZLL Inter-PAN operation interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    17.03.10 A. Taradov - Created.
******************************************************************************/
#ifndef _ZCLZLLINTERPAN_H
#define	_ZCLZLLINTERPAN_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zclZll.h>

/******************************************************************************
                    Definitions section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct _ZCL_ZllInterPanStartConf_t
{
  ZCL_ZllStatus_t         status;
} ZCL_ZllInterPanStartConf_t;

typedef struct _ZCL_ZllInterPanStartReq_t
{
  ZCL_ZllInterPanStartConf_t confirm;
  void (*ZCL_ZllInterPanStartConf)(ZCL_ZllInterPanStartConf_t *conf);
} ZCL_ZllInterPanStartReq_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Reset ZLL Inter-PAN module
******************************************************************************/
void ZCL_ZllInterPanReset(void);

/**************************************************************************//**
\brief Prepare stack for Inter-PAN operation

The function prepares the device for Inter-PAN communication. It is required 
for starting network scan (invoked by the ZCL_ZllScanReq() function).

The function is executed asynchronously. Invocation of the callback function
provided in the \c req->ZCL_ZllInterPanStartConf field indicates request
completion. If the request has been successful (the reported status equals
::ZCL_ZLL_SUCCESS_STATUS) ZigBee network features become unavailable.

To restore normal stack operation, when Inter-PAN data exchange has finished, 
call the ZDO_ResetNetworkReq() function.
******************************************************************************/
void ZCL_ZllInterPanStartReq(ZCL_ZllInterPanStartReq_t *req);

#endif // _ZCLZLLINTERPAN_H

// eof zclZllInterPan.h
