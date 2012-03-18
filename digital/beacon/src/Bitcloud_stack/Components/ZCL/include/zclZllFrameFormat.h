/**************************************************************************//**
  \file zclZllFrameFormat.h

  \brief
    Inter-PAN commands frame formats for ZLL.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    16.03.10 A. Taradov - Created.
******************************************************************************/
#ifndef _ZCLZLLFRAMEFORMAT_H
#define	_ZCLZLLFRAMEFORMAT_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <types.h>
#include <sspCommon.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define MAX_DEVICE_INFO_ENTRIES_NUMBER   1

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum _ZclZllCommandId_t
{
  ZCL_ZLL_SCAN_REQUEST_COMMAND_ID                    = 0x00,
  ZCL_ZLL_SCAN_RESPONSE_COMMAND_ID                   = 0x01,
  ZCL_ZLL_DEVICE_INFO_REQUEST_COMMAND_ID             = 0x02,
  ZCL_ZLL_DEVICE_INFO_RESPONSE_COMMAND_ID            = 0x03,
  ZCL_ZLL_IDENTIFY_REQUEST_COMMAND_ID                = 0x06,
  ZCL_ZLL_RESET_TO_FACTORY_NEW_REQUEST_COMMAND_ID    = 0x07,
  ZCL_ZLL_NETWORK_START_REQUEST_COMMAND_ID           = 0x10,
  ZCL_ZLL_NETWORK_START_RESPONSE_COMMAND_ID          = 0x11,
  ZCL_ZLL_NETWORK_JOIN_ROUTER_REQUEST_COMMAND_ID     = 0x12,
  ZCL_ZLL_NETWORK_JOIN_ROUTER_RESPONSE_COMMAND_ID    = 0x13,
  ZCL_ZLL_NETWORK_JOIN_ENDDEVICE_REQUEST_COMMAND_ID  = 0x14,
  ZCL_ZLL_NETWORK_JOIN_ENDDEVICE_RESPONSE_COMMAND_ID = 0x15,
  ZCL_ZLL_NETWORK_UPDATE_REQUEST_COMMAND_ID          = 0x16,
} ZclZllCommandId_t;

BEGIN_PACK
typedef struct PACK
{
  struct
  {
    uint8_t frameType            : 2; /*! Frame type sub-field */
    uint8_t manufacturerSpecific : 1; /*! Manufacturer specific sub-field */
    uint8_t direction            : 1; /*! Direction sub-field */
    uint8_t defaultResponse      : 1; /*! Disable default response sub-field */
    uint8_t reserved             : 3; /*! Reserved bits. Must have a zero value */
  } frameControl;
  uint8_t     seq;
  uint8_t     commandId;
} ZclZllFrameHeader_t;

typedef struct PACK
{
  uint8_t logicalType  : 2;
  uint8_t rxOnWhenIdle : 1;
  uint8_t reserved     : 5;
} ZclZllZigBeeInfo_t;

typedef struct PACK
{
  uint8_t factoryNew          : 1;
  uint8_t addressAssignment   : 1;
  uint8_t compoundDevice      : 1;
  uint8_t relayedScanRequest  : 1;
  uint8_t touchLinkInitiator  : 1;
  uint8_t touchLinkTimeWindow : 1; /*! This field is reserved in Scan Request frame */
  uint8_t reserved            : 2;
} ZclZllInfo_t;

typedef struct PACK
{
  uint64_t ieeeAddress;
  uint8_t  endpoint;
  uint16_t profileId;
  uint16_t deviceId;
  uint8_t  version;
  uint8_t  groupIds;
  uint8_t  sort;
} ZclZllDeviceInfoEntry_t;

typedef struct PACK
{
  ZclZllFrameHeader_t header;
  uint32_t            transactionId;
  ZclZllZigBeeInfo_t  zigBeeInfo;
  ZclZllInfo_t        zllInfo;
} ZclZllScanRequestFrame_t;

typedef struct PACK
{
  ZclZllFrameHeader_t header;
  uint32_t            transactionId;
  uint8_t             rssiCorrection;
  ZclZllZigBeeInfo_t  zigBeeInfo;
  ZclZllInfo_t        zllInfo;
  uint16_t            keyBitMask;
  uint32_t            responseId;
  uint64_t            extPanId;
  uint8_t             nwkUpdateId;
  uint8_t             channel;
  uint16_t            panId;
  uint16_t            networkAddress;
  uint8_t             numberSubDevices;
  uint8_t             totalGroupIds;
  // next fields are present only if numberSubDevices == 1
  union
  {
    struct
    {
      uint8_t         endpoint;
      uint16_t        profileId;
      uint16_t        deviceId;
      uint8_t         version;
      uint8_t         groupIds;
    };
    uint64_t          ieeeRelayerScanRequest1;
  };
  /*! This field is present only if zllInfo.relayedScanRequest == 1 */
  uint64_t            ieeeRelayerScanRequest2;
} ZclZllScanResponseFrame_t;

typedef struct PACK
{
  ZclZllFrameHeader_t header;
  uint32_t            transactionId;
  uint8_t             startIndex;
} ZclZllDeviceInfoRequestFrame_t;

typedef struct PACK
{
  ZclZllFrameHeader_t header;
  uint32_t            transactionId;
  uint8_t             numberSubDevices;
  uint8_t             startIndex;
  uint8_t             count;
  ZclZllDeviceInfoEntry_t entries[MAX_DEVICE_INFO_ENTRIES_NUMBER];
} ZclZllDeviceInfoResponseFrame_t;

typedef struct PACK
{
  ZclZllFrameHeader_t header;
  uint32_t            transactionId;
  uint16_t            identifyTime;
} ZclZllIdentifyRequestFrame_t;

typedef struct PACK
{
  ZclZllFrameHeader_t header;
  uint32_t            transactionId;
} ZclZllResetToFactoryNewRequestFrame_t;

typedef struct PACK
{
  ZclZllFrameHeader_t header;
  uint32_t            transactionId;
  uint64_t            extendedPanId;
  uint8_t             keyIndex;
  uint8_t             encryptedNwkKey[SECURITY_KEY_SIZE];
  uint8_t             channel;
  uint16_t            panId;
  uint16_t            nwkAddress;
  uint16_t            groupIdsBegin;
  uint16_t            groupIdsEnd;
  uint16_t            freeNwkAddressRangeBegin;
  uint16_t            freeNwkAddressRangeEnd;
  uint16_t            freeGroupIdRangeBegin;
  uint16_t            freeGroupIdRangeEnd;
  uint64_t            edIeeeAddress;
  uint16_t            edNwkAddress;
} ZclZllNetworkStartRequestFrame_t;

typedef struct PACK
{
  ZclZllFrameHeader_t header;
  uint32_t            transactionId;
  uint8_t             status;
  uint64_t            extendedPanId;
  uint8_t             nwkUpdateId;
  uint8_t             channel;
  uint16_t            panId;
} ZclZllNetworkStartResponseFrame_t;

typedef struct PACK
{
  ZclZllFrameHeader_t header;
  uint32_t            transactionId;
  uint64_t            extendedPanId;
  uint8_t             keyIndex;
  uint8_t             encryptedNwkKey[SECURITY_KEY_SIZE];
  uint8_t             nwkUpdateId;
  uint8_t             channel;
  uint16_t            panId;
  uint16_t            nwkAddress;
  uint16_t            groupIdsBegin;
  uint16_t            groupIdsEnd;
  uint16_t            freeNwkAddressRangeBegin;
  uint16_t            freeNwkAddressRangeEnd;
  uint16_t            freeGroupIdRangeBegin;
  uint16_t            freeGroupIdRangeEnd;
} ZclZllNetworkJoinRouterRequestFrame_t;

typedef struct PACK
{
  ZclZllFrameHeader_t header;
  uint32_t            transactionId;
  uint8_t             status;
} ZclZllNetworkJoinRouterResponseFrame_t;

typedef struct PACK
{
  ZclZllFrameHeader_t header;
  uint32_t            transactionId;
  uint64_t            extendedPanId;
  uint8_t             keyIndex;
  uint8_t             encryptedNwkKey[SECURITY_KEY_SIZE];
  uint8_t             nwkUpdateId;
  uint8_t             channel;
  uint16_t            panId;
  uint16_t            nwkAddress;
  uint16_t            groupIdsBegin;
  uint16_t            groupIdsEnd;
  uint16_t            freeNwkAddressRangeBegin;
  uint16_t            freeNwkAddressRangeEnd;
  uint16_t            freeGroupIdRangeBegin;
  uint16_t            freeGroupIdRangeEnd;
} ZclZllNetworkJoinEndDeviceRequestFrame_t;

typedef struct PACK
{
  ZclZllFrameHeader_t header;
  uint32_t            transactionId;
  uint8_t             status;
} ZclZllNetworkJoinEndDeviceResponseFrame_t;

typedef struct PACK
{
  ZclZllFrameHeader_t header;
  uint32_t            transactionId;
  uint64_t            extendedPanId;
  uint8_t             nwkUpdateId;
  uint8_t             channel;
  uint16_t            panId;
  uint16_t            nwkAddress;
} ZclZllNetworkUpdateRequestFrame_t;

END_PACK

#endif // _ZCLZLLFRAMEFORMAT_H

// eof zclZllFrameFormat.h
