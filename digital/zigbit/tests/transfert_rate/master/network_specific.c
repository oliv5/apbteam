/* network.c */
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

#include <types.h>
#include <configServer.h>
#include <zdo.h>

#include "configuration.h"
#include "network.h"
#include "network_specific.h"
#include "print.h"
#include "timer.h"

AppMessageBuffer_t zigbit_tx_buffer;

extern APS_RegisterEndpointReq_t endpointParams;
extern APS_DataReq_t network_config;





/* Specific callback after data packet sent */
void network_specific_DataConfcallback(void)
{
	static uint16_t count = 0;
	if(count < PACKET_NUMBER)
	{
		if(network_send_buffer_transfert_rate(NULL,0) == 1)
		{
			network_send_buffer_transfert_rate(NULL,0);
			count++;
			uprintf(".");
		}
	}
	else
	{
		timer_stop();
		uprintf("\r\nTemps ecoule = %.2f s, Baudrate = %.2f ko/s\r\n",timer_get_value_s(),(float)1024*(float)PACKET_NUMBER/timer_get_value_s()/1000);
	}
}


/* This function sends a specified buffer */
uint8_t network_send_buffer_transfert_rate(uint8_t * buffer_in,uint16_t size)
{
	static uint8_t * buffer;
	static uint16_t buffer_size;
	static uint16_t offset;
	uint8_t ASDUsize;
	uint16_t delta;
	
	if((buffer_in != NULL) && (size != 0)) 
	{
		buffer_size = size;
		buffer = buffer_in;
		offset = 0;
	}
	else
	{
		if(offset == buffer_size)
		{
			offset=0;
 			return 1;
		}
	}
	
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
		network_config.APS_DataConf = APS_DataConf;						// Confirm handler    Z
		
		delta = buffer_size - offset;
		
		if(delta > APP_MAX_PACKET_SIZE)
			ASDUsize = APP_MAX_PACKET_SIZE;
		else
			ASDUsize  = delta;
		
		memcpy(zigbit_tx_buffer.message.data,buffer+offset,ASDUsize);
		offset+=ASDUsize;
		
		/* Bitcloud sending request */
		network_set_transmission_state(APP_DATA_TRANSMISSION_WAIT_STATE);
		network_config.asduLength = ASDUsize + sizeof(zigbit_tx_buffer.message.messageId);		// actual application message length
		
		network_start_transmission();
	}
	return 0;
}

