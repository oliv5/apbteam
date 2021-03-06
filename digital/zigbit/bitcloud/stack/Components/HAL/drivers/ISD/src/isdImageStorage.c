/**************************************************************************//**
\file  isdImageStorage.c

\brief Implementation of image storage driver.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    25.05.11 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifdef _OTAU_
#if (APP_USE_OTAU == 1)

/******************************************************************************
                   Includes section
******************************************************************************/
#include <isdImageStorage.h>
#include <usart.h>
#include <rs232Controller.h>
#include <queue.h>
#include <dbg.h>
#include <appTimer.h>
#include <mnUtils.h>

/******************************************************************************
                   Defines section
******************************************************************************/
// Size of length field in frame
#define LEN_SIZE (sizeof(((IsdCommandFrame_t*)NULL)->length))
// Start of frame market
#define SOF 0x2A
// 500 ms interbyte timeout should be enough for everyone
#define INTERBYTE_TIMEOUT 500
// 5000 ms timeout between request to storage system and response from it.
#define INTERMESSAGE_TIMEOUT 5000
#define USART_RX_BUFFER_LENGTH 104

#ifndef APP_MAX_COMMAND_PAYLOAD_SIZE
  #define APP_MAX_COMMAND_PAYLOAD_SIZE 100
#endif

// code from runner
#define ISD_ZCL_COMMAND_INDICATION           0x0046
#define ISD_ZCL_COMMAND_REQUEST              0x0044
#define ISD_DRIVER_INIT_REQUEST              0x0100
#define ISD_DRIVER_INIT_RESPONSE             0x0101
#define ISD_COMMAND_ID_SIZE                  (sizeof(uint16_t))
#define ISD_INIT_MARKER                      0x55
/******************************************************************************
                   Types section
******************************************************************************/
// States of tx FSM
typedef enum _TxState_t
{
  TX_ERR_OR_OFF,
  TX_IDLE,
  TX_SENDING_SOF,
  TX_SENDING_DATA,
} TxState_t;

// States of rx FSM
typedef enum _RxState_t
{
  RX_ERR_OR_OFF,
  RX_WAITING_START,   // AKA IDLE
  RX_WAITING_LEN,
  RX_WAITING_DATA,
} RxState_t;

BEGIN_PACK
typedef struct PACK _IsdCommandFrame_t
{
  uint16_t   length;
  uint16_t   commandId;
  uint8_t    payload[APP_MAX_COMMAND_PAYLOAD_SIZE];
} IsdCommandFrame_t;

typedef struct PACK
{
  uint8_t     srcAddrMode;
  ShortAddr_t srcShortAddress;
  ExtAddr_t   srcExtAddress;
  ProfileId_t srcProfileId;
  Endpoint_t  srcEndpointId;
  ClusterId_t srcClusterId;

  ClusterId_t clusterId;
  uint8_t     direction;
  uint8_t     commandId;
  uint8_t     payload[1];
} IsdCommandIndication_t;

typedef struct PACK
{
  LITTLE_ENDIAN_OCTET(3,(
    uint8_t direction : 1,
    uint8_t generalCommand : 1,
    uint8_t reserved : 6
  ))
} IsdCommandOptions_t;

typedef struct PACK
{
  uint8_t addrMode;
  ShortAddr_t shortAddress;
  ExtAddr_t extAddress;
  ProfileId_t profileId;
  uint8_t endpoint;
  uint8_t dstEndpoint;
  ClusterId_t clusterId;
  uint8_t defaultResponse;
  IsdCommandOptions_t commandOptions;
  uint8_t commandId;
  uint8_t recordsCount;
  uint8_t request[1];
} IsdCommandRequest_t;
END_PACK

/******************************************************************************
                   Static Function Prototypes section
******************************************************************************/
static void isdUsartReceivedHandler(uint16_t bytesAmount);
static void isdSendCommandFrame(void);
static void isdUsartTransmittedHandler(void);
static void isdInterbyteTimeoutExpired(void);
static void isdIntermessageTimeoutExpired(void);
static void isdInitReq(void);
static void isdInitResp(void);
static void isdSerialNotify(void);
static void isdSetState(ISD_Status_t state);

/******************************************************************************
                   Static variables section
******************************************************************************/
static HAL_UsartDescriptor_t usartDescriptor;
static RxState_t rxState = RX_ERR_OR_OFF;
static TxState_t txState = TX_ERR_OR_OFF;
static uint8_t usartDescriptorRxBuffer[USART_RX_BUFFER_LENGTH];
static IsdCommandFrame_t isdBuffer;

static HAL_AppTimer_t interbyteTimer =
{
  .interval = INTERBYTE_TIMEOUT,
  .mode = TIMER_ONE_SHOT_MODE,
  .callback = isdInterbyteTimeoutExpired,
};

static HAL_AppTimer_t intermessageTimer =
{
  .interval = INTERMESSAGE_TIMEOUT,
  .mode = TIMER_ONE_SHOT_MODE,
  .callback = isdIntermessageTimeoutExpired,
};

static IsdOpenCb_t generalCb;
static IsdUpgradeEndCb_t upgradeEndCb;
static IsdQueryNextImageCb_t queryNextImageCb;
static IsdImageBlockCb_t imageBlockCb;

static ISD_Status_t isdState;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Open image storage driver

\param[in] cb - callback about driver actions
******************************************************************************/
void ISD_Open(IsdOpenCb_t cb)
{
  // Implementation depends on 'length' field in IsdCommandFrame_t being one byte
  // In case of change, several modifications would be required
  assert_static(2 == LEN_SIZE);
  // Also depends on 'length' field being first field of command frame
  assert_static(0 == offsetof(IsdCommandFrame_t, length));

#ifdef BSP_ENABLE_RS232_CONTROL
  BSP_EnableRs232();
#endif /* BSP_ENABLE_RS232_CONTROL */

  usartDescriptor.tty            = APP_USART_CHANNEL;
  usartDescriptor.mode           = USART_MODE_ASYNC;
  usartDescriptor.flowControl    = USART_FLOW_CONTROL_NONE;
  usartDescriptor.baudrate       = USART_BAUDRATE_38400;
  usartDescriptor.dataLength     = USART_DATA8;
  usartDescriptor.parity         = USART_PARITY_NONE;
  usartDescriptor.stopbits       = USART_STOPBIT_1;
  usartDescriptor.rxBuffer       = usartDescriptorRxBuffer;
  usartDescriptor.rxBufferLength = USART_RX_BUFFER_LENGTH; //BULK_SIZE
  usartDescriptor.txBuffer       = NULL;
  usartDescriptor.txBufferLength = 0;
  usartDescriptor.rxCallback     = isdUsartReceivedHandler;
  usartDescriptor.txCallback     = isdUsartTransmittedHandler;

  generalCb = cb;

  if (-1 != HAL_OpenUsart(&usartDescriptor))
  {
    rxState = RX_WAITING_START;
    txState = TX_IDLE;
    isdInitReq();
  }
  else
  {
    rxState = RX_ERR_OR_OFF;
    txState = TX_ERR_OR_OFF;
    isdSetState(ISD_HARDWARE_FAULT);
  }
}

/**************************************************************************//**
\brief Request to storage system about communication.
******************************************************************************/
static void isdInitReq(void)
{
  isdBuffer.commandId = ISD_DRIVER_INIT_REQUEST;
  isdBuffer.length = ISD_COMMAND_ID_SIZE + sizeof(uint8_t);
  isdBuffer.payload[0] = ISD_INIT_MARKER;

  HAL_StartAppTimer(&intermessageTimer);
  isdSendCommandFrame();
}

/**************************************************************************//**
\brief Response from storage system about communication.
******************************************************************************/
static void isdInitResp(void)
{
  HAL_StopAppTimer(&intermessageTimer);

  if (ISD_INIT_MARKER != isdBuffer.payload[0])
    isdSetState(ISD_COMMUNICATION_LOST);
  else
    isdSetState(ISD_SUCCESS);
}

/**************************************************************************//**
\brief Close image storage driver
******************************************************************************/
void ISD_Close(void)
{
  rxState = RX_ERR_OR_OFF;
  txState = TX_ERR_OR_OFF;
  HAL_CloseUsart(&usartDescriptor);
}

/**************************************************************************//**
\brief Reset rx FSM on expiration of interbyte timeout
******************************************************************************/
static void isdInterbyteTimeoutExpired(void)
{
  if (RX_ERR_OR_OFF != rxState)
    rxState = RX_WAITING_START;
}

/**************************************************************************//**
\brief Callback about timout expiration between request and response
******************************************************************************/
static void isdIntermessageTimeoutExpired(void)
{
  isdSetState(ISD_COMMUNICATION_LOST);
  isdInitReq();
}

/**************************************************************************//**
\brief Callback on reception of serial byte(s)

\param[in] bytesAmount number of received bytes
******************************************************************************/
static void isdUsartReceivedHandler(uint16_t bytesAmount)
{
  static uint16_t rxCnt = 0;
  while (bytesAmount)
  {
    switch (rxState)
    {
      case RX_WAITING_START:
        {
          uint8_t data;
          if (HAL_ReadUsart(&usartDescriptor, &data, sizeof(uint8_t)) <= 0)
          {
            rxState = RX_ERR_OR_OFF;
            isdSetState(ISD_HARDWARE_FAULT);
            return;
          }
          if (SOF == data)
            rxState = RX_WAITING_LEN;

          bytesAmount--;
        }
        break;

      case RX_WAITING_LEN:
        {
          if (bytesAmount >= sizeof(uint16_t))
          {
            if (HAL_ReadUsart(&usartDescriptor, (uint8_t *)&isdBuffer.length, sizeof(uint16_t)) <= 0)
            {
              rxState = RX_ERR_OR_OFF;
              isdSetState(ISD_HARDWARE_FAULT);
              return;
            }

            // Limit length to avoid possible buffer overflow
            isdBuffer.length = MIN(isdBuffer.length, sizeof(IsdCommandFrame_t) - LEN_SIZE);

            if (isdBuffer.length > 0)
              rxState = RX_WAITING_DATA;
            else
              rxState = RX_WAITING_START; // Do not allow zero len

            bytesAmount -= sizeof(uint16_t);
            rxCnt = 0;
          }
          else
            return;
        }
        break;

      case RX_WAITING_DATA:
        {
          int readCnt;
          uint8_t *readPtr = (uint8_t *)&isdBuffer;

          readPtr += offsetof(IsdCommandFrame_t, commandId) + rxCnt;
          readCnt = HAL_ReadUsart(&usartDescriptor, readPtr, MIN(bytesAmount, (uint16_t) (isdBuffer.length - rxCnt)));

          if (readCnt <= 0)
          {
            rxState = RX_ERR_OR_OFF;
            isdSetState(ISD_HARDWARE_FAULT);
            return;
          }

          bytesAmount -= readCnt;
          rxCnt += readCnt;
          if (rxCnt >= isdBuffer.length)
          {
            // Full packet is received
            rxState = RX_WAITING_START;
            isdSerialNotify();
          }
        }
        break;

      case RX_ERR_OR_OFF:
      default:
        return;
    }
  }

  HAL_StopAppTimer(&interbyteTimer);

  if (RX_WAITING_DATA == rxState || RX_WAITING_LEN == rxState)
    HAL_StartAppTimer(&interbyteTimer);
}

/**************************************************************************//**
\brief Sends new command frame

\return result code
******************************************************************************/
static void isdSendCommandFrame(void)
{
  static uint8_t sof = SOF;

  if (HAL_WriteUsart(&usartDescriptor, &sof, sizeof(sof)) > 0)
  {
    txState = TX_SENDING_SOF;
  }
  else
  {
    txState = TX_ERR_OR_OFF;
    isdSetState(ISD_HARDWARE_FAULT);
  }
}

/**************************************************************************//**
\brief Callback on completion of single serial transmit
******************************************************************************/
static void isdUsartTransmittedHandler(void)
{
  switch (txState)
  {
    case TX_SENDING_SOF:
      {
        uint8_t *writePtr = (uint8_t *)&isdBuffer;

        writePtr += offsetof(IsdCommandFrame_t, length);
        if (HAL_WriteUsart(&usartDescriptor, writePtr, isdBuffer.length + LEN_SIZE) <= 0)
        {
          txState = TX_ERR_OR_OFF;
          isdSetState(ISD_HARDWARE_FAULT);
        }
        else
          txState = TX_SENDING_DATA;
      }
      break;

    case TX_SENDING_DATA:
        txState = TX_IDLE;
      break;

    case TX_IDLE:
    case TX_ERR_OR_OFF:
    default:
      break;
  }
}

/**************************************************************************//**
\brief Send query next image request to storage system

\param[in] addressing - pointer to structure that include client network information; \n
\param[in] data - data payload; \n
\param[in] cd - callback about response receiving from storage system.
******************************************************************************/
void ISD_QueryNextImageReq(ZCL_Addressing_t *addressing, ZCL_OtauQueryNextImageReq_t *data, IsdQueryNextImageCb_t cb)
{
  IsdCommandIndication_t *serialData = (IsdCommandIndication_t *)isdBuffer.payload;

  if (ISD_SUCCESS != isdState)
    return;

  serialData->clusterId       = addressing->clusterId;
  serialData->commandId       = QUERY_NEXT_IMAGE_REQUEST_ID;
  serialData->srcAddrMode     = addressing->addrMode;
  serialData->srcShortAddress = addressing->addr.shortAddress;
  serialData->srcExtAddress   = addressing->addr.extAddress;
  serialData->srcProfileId    = addressing->profileId;
  serialData->srcEndpointId   = addressing->endpointId;
  serialData->srcClusterId    = addressing->clusterId;
  serialData->direction       = addressing->clusterSide;

  memcpy(serialData->payload, data, sizeof(ZCL_OtauQueryNextImageReq_t));

  isdBuffer.commandId = ISD_ZCL_COMMAND_INDICATION;
  isdBuffer.length = ISD_COMMAND_ID_SIZE  + sizeof(IsdCommandIndication_t) +
      sizeof(ZCL_OtauQueryNextImageReq_t) - sizeof(uint8_t);

  queryNextImageCb = cb;

  HAL_StartAppTimer(&intermessageTimer);
  isdSendCommandFrame();
}

/**************************************************************************//**
\brief Send image block request to storage system

\param[in] addressing - pointer to structure that include client network information; \n
\param[in] data - data payload; \n
\param[in] cd - callback about response receiving from storage system.
******************************************************************************/
void ISD_ImageBlockReq(ZCL_Addressing_t *addressing, ZCL_OtauImageBlockReq_t *data, IsdImageBlockCb_t cb)
{
  IsdCommandIndication_t *serialData = (IsdCommandIndication_t *)isdBuffer.payload;

  if (ISD_SUCCESS != isdState)
    return;

  serialData->clusterId       = addressing->clusterId;
  serialData->commandId       = IMAGE_BLOCK_REQUEST_ID;
  serialData->srcAddrMode     = addressing->addrMode;
  serialData->srcShortAddress = addressing->addr.shortAddress;
  serialData->srcExtAddress   = addressing->addr.extAddress;
  serialData->srcProfileId    = addressing->profileId;
  serialData->srcEndpointId   = addressing->endpointId;
  serialData->srcClusterId    = addressing->clusterId;
  serialData->direction       = addressing->clusterSide;

  memcpy(serialData->payload, data, sizeof(ZCL_OtauImageBlockReq_t));

  isdBuffer.commandId = ISD_ZCL_COMMAND_INDICATION;
  isdBuffer.length = ISD_COMMAND_ID_SIZE  + sizeof(IsdCommandIndication_t) +
      sizeof(ZCL_OtauImageBlockReq_t) - sizeof(uint8_t);

  imageBlockCb = cb;

  HAL_StartAppTimer(&intermessageTimer);
  isdSendCommandFrame();
}

/**************************************************************************//**
\brief Send upgrade end request to storage system

\param[in] addressing - pointer to structure that include client network information; \n
\param[in] data - data payload; \n
\param[in] cd - callback about response receiving from storage system.
******************************************************************************/
void ISD_UpgradeEndReq(ZCL_Addressing_t *addressing, ZCL_OtauUpgradeEndReq_t *data, IsdUpgradeEndCb_t cb)
{
  IsdCommandIndication_t *serialData = (IsdCommandIndication_t *)isdBuffer.payload;

  if (ISD_SUCCESS != isdState)
    return;

  serialData->clusterId       = addressing->clusterId;
  serialData->commandId       = UPGRADE_END_REQUEST_ID;
  serialData->srcAddrMode     = addressing->addrMode;
  serialData->srcShortAddress = addressing->addr.shortAddress;
  serialData->srcExtAddress   = addressing->addr.extAddress;
  serialData->srcProfileId    = addressing->profileId;
  serialData->srcEndpointId   = addressing->endpointId;
  serialData->srcClusterId    = addressing->clusterId;
  serialData->direction       = addressing->clusterSide;

  memcpy(serialData->payload, data, sizeof(ZCL_OtauUpgradeEndReq_t));

  isdBuffer.commandId = ISD_ZCL_COMMAND_INDICATION;
  isdBuffer.length = ISD_COMMAND_ID_SIZE  + sizeof(IsdCommandIndication_t) +
      sizeof(ZCL_OtauUpgradeEndReq_t) - sizeof(uint8_t);

  upgradeEndCb = cb;

  HAL_StartAppTimer(&intermessageTimer);
  isdSendCommandFrame();
}

/**************************************************************************//**
\brief Receive any messages from storage system
******************************************************************************/
static void isdSerialNotify(void)
{
  IsdCommandRequest_t *req = (IsdCommandRequest_t *)isdBuffer.payload;

  if (ISD_DRIVER_INIT_RESPONSE == isdBuffer.commandId)
  {
     isdInitResp();
     return;
  }

  if (ISD_ZCL_COMMAND_REQUEST != isdBuffer.commandId)
    return;

  if (OTAU_CLUSTER_ID != req->clusterId)
    return;

  switch (req->commandId)
  {
    case QUERY_NEXT_IMAGE_RESPONSE_ID:
      {
        ZCL_OtauQueryNextImageResp_t *resp = (ZCL_OtauQueryNextImageResp_t *)req->request;

        HAL_StopAppTimer(&intermessageTimer);
        if (queryNextImageCb)
        {
          queryNextImageCb(resp);
          queryNextImageCb = NULL;
        }
      }
      break;
    case IMAGE_BLOCK_RESPONSE_ID:
      {
        ZCL_OtauImageBlockResp_t *resp = (ZCL_OtauImageBlockResp_t *)req->request;

        HAL_StopAppTimer(&intermessageTimer);
        if (imageBlockCb)
        {
          imageBlockCb(resp);
          imageBlockCb = NULL;
        }
      }
      break;
    case UPGRADE_END_RESPONSE_ID:
      {
        ZCL_OtauUpgradeEndResp_t *resp = (ZCL_OtauUpgradeEndResp_t *)req->request;

        HAL_StopAppTimer(&intermessageTimer);
        if (upgradeEndCb)
        {
          upgradeEndCb(resp);
          upgradeEndCb = NULL;
        }
        else
        {
          ZCL_Addressing_t addr =
          {
            .addrMode             = req->addrMode,
            .profileId            = req->profileId,
            .endpointId           = req->dstEndpoint,
            .clusterId            = req->clusterId,
            .clusterSide          = ZCL_CLIENT_CLUSTER_TYPE,
            .manufacturerSpecCode = 0,
          };

          if (APS_EXT_ADDRESS == addr.addrMode)
            addr.addr.extAddress = req->extAddress;
          else
            addr.addr.shortAddress = req->shortAddress;

          ZCL_UnsolicitedUpgradeEndResp(&addr, resp);
        }
      }
      break;
  }
}

/**************************************************************************//**
\brief Set actual driver state and report to high layer about that.

\param[in] state - actual driver state
******************************************************************************/
static void isdSetState(ISD_Status_t state)
{
  isdState = state;

  if (generalCb)
    generalCb(state);
}

#endif // (APP_USE_OTAU == 1)
#endif // _OTAU_

// eof isdImageStorage.c
