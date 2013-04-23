/* network_send_commands.c */
/* generic messages to send. {{{
 *
 * Copyright (C) 2013 Florent Duchon
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

#include "network.h"

AppMessageBuffer_t zigbit_tx_buffer;


extern APS_RegisterEndpointReq_t endpointParams;
extern APS_DataReq_t network_config;
extern SimpleDescriptor_t simpleDescriptor;


/* This function applies the generic configuration requested before sending any packet */
void network_TX_prepare_configuration(void)
{
	if(network_get_state() == APP_NETWORK_JOINED_STATE)
	{
		/* Configure the message structure */
		network_config.dstAddrMode = APS_SHORT_ADDRESS;					// Short addressing mode
		network_config.profileId = simpleDescriptor.AppProfileId;						// Profile ID
		network_config.dstEndpoint = simpleDescriptor.endpoint;						// Desctination endpoint
		network_config.clusterId = APP_CLUSTER_ID;						// Desctination cluster ID
		network_config.srcEndpoint = simpleDescriptor.endpoint;						// Source endpoint
		network_config.txOptions.acknowledgedTransmission = 0;				// Acknowledged transmission enabled
		network_config.txOptions.fragmentationPermitted = 0;
		network_config.radius = 0;										// Use maximal possible radius
		network_config.APS_DataConf = APS_DataConf;	
	}
}


/* This function must be used to send angle through zigbee network */
void network_send_angle(uint16_t address,uint16_t angle)
{
	if(network_get_state() == APP_NETWORK_JOINED_STATE)
	{
		/* Configure network for tx */
		network_TX_prepare_configuration();
		
		/* Destination */
		network_config.dstAddress.shortAddress = address;
		
		/* Message Type */
		zigbit_tx_buffer.message.type = NETWORK_ANGLE_RAW;
		
		/* LSB Data */
		zigbit_tx_buffer.message.data[NETWORK_MSG_DATA_LSB_FIELD] = angle;
		
		/* MSB Data */
		zigbit_tx_buffer.message.data[NETWORK_MSG_DATA_MSB_FIELD] = angle >> 8;
		
		
		/* Bitcloud sending request */
		network_config.asdu = (uint8_t*) &zigbit_tx_buffer.message;					// application message pointer
		network_config.asduLength = 2 + sizeof(zigbit_tx_buffer.message.type);		// actual application message length
		
		network_start_transmission();
	}
}


/* This function must be used to send uart frame over zigbee network */
void network_send_buffer_over_zb(uint16_t address,uint8_t * buffer,uint8_t len)
{
	static uint8_t current_ASDUsize = 0;
	if(network_get_state() == APP_NETWORK_JOINED_STATE)
	{
		if(network_get_transmission_state() == APP_DATA_TRANSMISSION_READY_STATE)
		{
			current_ASDUsize = 0;
		}
		
		/* Configure network for tx */
		network_TX_prepare_configuration();
		
		/* Destination */
		network_config.dstAddress.shortAddress = address;
		
		/* Message Type */
		zigbit_tx_buffer.message.type = NETWORK_UART_OVER_ZB;
		
		memcpy(zigbit_tx_buffer.message.data+current_ASDUsize,buffer,len);
		current_ASDUsize += len;
		
		/* Bitcloud sending request */
		network_config.asdu = (uint8_t*) &zigbit_tx_buffer.message;					// application message pointer
		network_config.asduLength = current_ASDUsize + sizeof(zigbit_tx_buffer.message.type);		// actual application message length
		
		network_start_transmission();
	}
}


/* This function must be used to send reset command through zigbee network */
void network_send_reset(uint16_t address)
{
	
	if(network_get_state() == APP_NETWORK_JOINED_STATE)
	{
		/* Configure network for tx */
		network_TX_prepare_configuration();
		network_config.dstAddress.shortAddress = address;						// Destination address	
		network_config.asdu = (uint8_t*) &zigbit_tx_buffer.message;					// application message pointer
		
		/* Message type*/
		zigbit_tx_buffer.message.type = NETWORK_RESET;
		
		/* Bitcloud sending request */
		network_config.asduLength = sizeof(zigbit_tx_buffer.message.type);		// actual application message length
		
		network_start_transmission();
	}
}


/* This function must be used to send jack state through zigbee network */
void network_send_jack_state(uint16_t address,uint8_t state)
{
	
	if(network_get_state() == APP_NETWORK_JOINED_STATE)
	{
		/* Configure network for tx */
		network_TX_prepare_configuration();
		network_config.dstAddress.shortAddress = address;						// Destination address	
		network_config.asdu = (uint8_t*) &zigbit_tx_buffer.message;					// application message pointer
		
		/* Message type*/
		zigbit_tx_buffer.message.type = NETWORK_JACK_STATE;
		
		/* Source address */
		zigbit_tx_buffer.message.data[NETWORK_MSG_DATA_MSB_FIELD] = state;
		
		/* Bitcloud sending request */
		network_config.asduLength = 1 + sizeof(zigbit_tx_buffer.message.type);		// actual application message length
		
		network_start_transmission();
	}
}


