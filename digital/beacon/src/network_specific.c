/* network_specific.c */
/* Beacon specifical network management. {{{
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




#include <types.h>
#include <configServer.h>
#include <zdo.h>

#include "configuration.h"
#include "network.h"
#include "network_specific.h"
#include "print.h"
#include "motor.h"
#include "position.h"
#include "codewheel.h"
#include "servo.h"
#include "reset.h"

AppMessageBuffer_t zigbit_tx_buffer;

extern APS_RegisterEndpointReq_t endpointParams;
extern APS_DataReq_t network_config;


/* Specific callback after data packet received */
void network_specific_DataIndicationcallback(APS_DataInd_t* indData)
{
	uint8_t beacon = 0;
	uint16_t angle = 0;
	uint16_t angle_id = 0;
	AppMessage_t *appMessage = (AppMessage_t *) indData->asdu;
	
	/* Data received indication */
	switch(appMessage->data[NETWORK_MSG_TYPE_FIELD])
	{
		case NETWORK_JACK_STATE:
			if(appMessage->data[NETWORK_MSG_DATA_LSB_FIELD])
			{
				motor_start();
				servo_start_wave_task();
			}
			else
			{
				motor_stop();
				servo_stop_wave_task();
			}
			break;
		case NETWORK_OPPONENT_NUMBER:
			break;
		case NETWORK_ANGLE_RAW:
			
			/* Beacon address */
			beacon = appMessage->data[NETWORK_MSG_ADDR_FIELD];
			
			/* Angle ID */
			angle_id = appMessage->data[NETWORK_MSG_DATA_MSB_FIELD] >> 1;
			
			/* Angle value */
			angle = ((appMessage->data[NETWORK_MSG_DATA_MSB_FIELD]&0x01) << 8) + appMessage->data[NETWORK_MSG_DATA_LSB_FIELD];
			
			/* New angle is avaiiable, update position */
			update_position(beacon,angle_id,codewheel_convert_angle_raw2radians(angle));
			break;
		case NETWORK_RESET:
			reset_avr();
			break;
		default:
			uprintf("Unknown data type received = %x\r\n",appMessage->data[NETWORK_MSG_TYPE_FIELD]);
			break;
	}
}

/* Specific callback after data packet sent */
void network_specific_DataConfcallback(void)
{
}


/* This function must be used to send data through zigbee network */
void network_send_data(TMessage_type type, uint16_t data)
{
	
	if(network_get_state() == APP_NETWORK_JOINED_STATE)
	{
		/* Configure the message structure */
		network_config.dstAddrMode = APS_SHORT_ADDRESS;					// Short addressing mode
		network_config.dstAddress.shortAddress = 0x00;						// Destination address	
		network_config.profileId = APP_PROFILE_ID;						// Profile ID
		network_config.dstEndpoint = APP_ENDPOINT;						// Desctination endpoint
		network_config.clusterId = APP_CLUSTER_ID;						// Desctination cluster ID
		network_config.srcEndpoint = APP_ENDPOINT;						// Source endpoint
		network_config.asdu = (uint8_t*) &zigbit_tx_buffer.message;					// application message pointer
		network_config.txOptions.acknowledgedTransmission = 0;				// Acknowledged transmission enabled
		network_config.radius = 0;										// Use maximal possible radius
		network_config.APS_DataConf = APS_DataConf;	
		
		/* Message type*/
		zigbit_tx_buffer.message.data[NETWORK_MSG_TYPE_FIELD] = type;
		
		/* Source address */
		zigbit_tx_buffer.message.data[NETWORK_MSG_ADDR_FIELD] = CS_NWK_ADDR;
		
		/* LSB Data */
		zigbit_tx_buffer.message.data[NETWORK_MSG_DATA_LSB_FIELD] = data;
		
		/* MSB Data */
		zigbit_tx_buffer.message.data[NETWORK_MSG_DATA_MSB_FIELD] = data >> 8;
		
		/* Bitcloud sending request */
		network_set_transmission_state(APP_DATA_TRANSMISSION_WAIT_STATE);
		network_config.asduLength = 4 + sizeof(zigbit_tx_buffer.message.messageId);		// actual application message length
		
		network_start_transmission();
	}
}
