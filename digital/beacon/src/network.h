/**************************************************************************//**
  \file Peer2Peer.h

  \brief Peer-2-peer sample application header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    14.10.09 A. Taradov - Added FIFO for received packets
******************************************************************************/

#ifndef _NETWORK_H
#define _NETWORK_H

// #define MAX_RETRIES_BEFORE_REJOIN 6

// typedef struct
// {
// 	uint8_t header[APS_ASDU_OFFSET];
// 	uint8_t data;
// 	uint8_t footer[APS_AFFIX_LENGTH - APS_ASDU_OFFSET];
// }buff_t;


typedef struct
{
  uint8_t messageId;                           // message ID
  uint8_t data[APP_MAX_PACKET_SIZE];           // data
}AppMessage_t;

// Application network messsage buffer descriptor
typedef struct
{
  uint8_t header[APS_ASDU_OFFSET];                    // Auxiliary header (required by stack)
  AppMessage_t message;                               // Application message
  uint8_t footer[APS_AFFIX_LENGTH - APS_ASDU_OFFSET]; // Auxiliary footer (required by stack)
}AppMessageBuffer_t;




void APS_DataIndication(APS_DataInd_t* dataInd);
void APS_DataConf(APS_DataConf_t* confInfo);
void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t* confirmInfo);
void initNetwork(void);
void startNetwork(void);
void send_data(void);
void send_angle(int angle_degree);

#endif // ifndef _NETWORK_H

// eof network.h
