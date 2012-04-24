/* network.h */
/* Beacon network management. {{{
 *
 * Copyright (C) 2012 Florent Duchon
 *
 * APBTeam:
 *        Web: http://apbteam.org/
 *      Email: team AT apbteam DOT org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * }}} */

#ifndef _NETWORK_H
#define _NETWORK_H

#include <zdo.h>

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
void ZDO_UnbindIndication(ZDO_UnbindInd_t *unbindInd);
void ZDO_BindIndication(ZDO_BindInd_t *bindInd);
void ZDO_WakeUpInd(void);

void network_init(void);
void network_start(void);

/* This function returns the network status */
uint16_t network_get_status(void);

void send_data(uint8_t type, uint8_t data);
void send_angle(int angle_degree);
void network_leave(void);
/* Leave network response */
void zdpLeaveResp(ZDO_ZdpResp_t *zdpResp);

/* This function returns the LQI of the joined network */
uint8_t network_get_lqi(void);

/* This function returns the RSSI of the joined network */
int8_t network_get_rssi(void);



#endif // ifndef _NETWORK_H

// eof network.h
