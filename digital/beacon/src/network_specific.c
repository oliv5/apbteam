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
#include "uid.h"

/* Specific callback after data packet received */
void network_specific_DataIndicationcallback(APS_DataInd_t* indData)
{
	uint8_t beacon = 0;
	uint16_t angle = 0;
	uint16_t angle_id = 0;
	
	AppMessage_t *FrameReceived = (AppMessage_t *) indData->asdu;

	/* Data received indication */
	switch(FrameReceived->type)
	{
		case NETWORK_JACK_STATE:
			if(FrameReceived->data[NETWORK_MSG_DATA_MSB_FIELD])
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
			beacon = indData->srcAddress.shortAddress;
			/* Angle ID */
			angle_id = FrameReceived->data[NETWORK_MSG_DATA_MSB_FIELD] >> 1;
			
			/* Angle value */
			angle = ((FrameReceived->data[NETWORK_MSG_DATA_MSB_FIELD]&0x01) << 8) + FrameReceived->data[NETWORK_MSG_DATA_LSB_FIELD];
			
			/* New angle is avaiiable, update position */
			update_position(beacon,angle_id,codewheel_convert_angle_raw2radians(angle));
			break;
		case NETWORK_RESET:
			reset_avr();
			break;
		case NETWORK_UART_OVER_ZB:
			print_raw_data(FrameReceived->data,indData->asduLength-1);
			break;
		default:
			uprintf("Unknown data type received = %x\r\n",FrameReceived->type);
			break;
	}
}

/* Specific callback after data packet sent */
void network_specific_DataConfcallback(void)
{
}



