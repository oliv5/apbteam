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

BEGIN_PACK
typedef struct
{
	uint8_t type;                           		// message ID
	uint8_t data[APP_MAX_PACKET_SIZE];           // data
} AppMessage_t;
END_PACK

BEGIN_PACK
typedef struct
{
	uint8_t header[APS_ASDU_OFFSET];					// Auxiliary header (required by stack)
	AppMessage_t message;                               				// Application message
	uint8_t footer[APS_AFFIX_LENGTH - APS_ASDU_OFFSET]; 	// Auxiliary footer (required by stack)
} AppMessageBuffer_t;
END_PACK

typedef enum
{
	NETWORK_JACK_STATE,
	NETWORK_OPPONENT_NUMBER,
	NETWORK_ANGLE_RAW,
	NETWORK_RESET,
	NETWORK_UART_OVER_ZB,
	NETWORK_START_CALIBRATION
} TMessage_type;


typedef enum
{
	NETWORK_MSG_DATA_MSB_FIELD,
	NETWORK_MSG_DATA_LSB_FIELD
} TMessage_field;


/* This function intializes network parameters */
void network_init(uint16_t uid);

/* This function starts the network according to the defined configuraiton*/
void network_start(void);

/* ZDO_StartNetwork primitive confirmation callback */
void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t* confirmInfo);

/* This function sets the network status */
void network_set_state(AppState_t state);

/* This function returns the network status */
AppState_t network_get_state(void);

/* Update network status event handler */
void ZDO_MgmtNwkUpdateNotf(ZDO_MgmtNwkUpdateNotf_t *nwkParams);

/* This function returns the LQI of the joined network */
uint8_t network_get_lqi(ShortAddr_t nodeAddr);

/* This function returns the RSSI of the joined network */
int8_t network_get_rssi(ShortAddr_t nodeAddr);

/* APS data indication handler */
void APS_DataIndication(APS_DataInd_t* indData);

/* brief Handler of aps data sent confirmation */
void APS_DataConf(APS_DataConf_t* confInfo);

/* This function starts the transmission of a configured packet */
void network_start_transmission(void);

/* This function sets the transmission state s*/
void network_set_transmission_state(AppDataTransmissionState_t state);

/* This function returns the transmission state */
AppDataTransmissionState_t network_get_transmission_state(void);

/* This function quits the joined network */
void network_leave(void);

/* Leave network response */
void zdpLeaveResp(ZDO_ZdpResp_t *zdpResp);

/* Wakeup event handler (dummy) */
void ZDO_WakeUpInd(void);

/* Stub for ZDO Binding Indication */
void ZDO_BindIndication(ZDO_BindInd_t *bindInd);

/* Stub for ZDO Unbinding Indication */
void ZDO_UnbindIndication(ZDO_UnbindInd_t *unbindInd);


#endif
