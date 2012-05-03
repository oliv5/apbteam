/* main_avr.c */
/* Beacon State Machine & Main. {{{
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
#include <util/delay.h>
#include "configuration.h"
#include "calibration.h"
#include "codewheel.h"
#include "debug_avr.h"
#include "network.h"
#include "position.h"
#include "motor.h"
#include "laser.h"
#include "servo.h"
#include "led.h"
#include "twi.h"

// Application related parameters
AppState_t appState = APP_INITIAL_STATE;  // application state

#ifdef TYPE_COOR
	DeviceType_t deviceType = DEVICE_TYPE_COORDINATOR;
#else
 	DeviceType_t deviceType = DEVICE_TYPE_END_DEVICE;
#endif
	

void twi_RXTX_update(void)
{
	uint8_t TXbuffer[AC_TWI_SLAVE_SEND_BUFFER_SIZE];
	uint8_t RXbuffer[AC_TWI_SLAVE_RECV_BUFFER_SIZE];
	uint8_t RXlen;
	
	/* data to be communicated to the master */
	twi_slave_update (TXbuffer, sizeof (TXbuffer));
	
	/* Check for data. */
	RXlen = twi_slave_poll (RXbuffer, AC_TWI_SLAVE_RECV_BUFFER_SIZE);
	
	/* data availlable */
	if(RXlen != 0)
	{
	}
}

void APL_TaskHandler(void)
{
	if(deviceType == DEVICE_TYPE_COORDINATOR)
	{
		twi_RXTX_update();
	}
	switch (appState)
	{
		case APP_INITIAL_STATE:
			
			/* Init Led */
			init_led();
			
			/* Init Serial Interface for debug */ 
  			initSerialInterface();          
			
			switch(deviceType)
			{
				case DEVICE_TYPE_COORDINATOR:
						network_init();
						twi_init(AC_BEACON_TWI_ADDRESS);
						uprintf("COORDINATOR initialisation OK !\n\r");
					break;
				case DEVICE_TYPE_END_DEVICE:
						servo_init();
						codewheel_init();
						laser_init();
 						network_init();
						motor_init();
						uprintf("LOL_%d initialisation OK !\n\r",CS_NWK_ADDR);
						calibration_start_task();
					break;
				default:
					break;
			}
 			appState = APP_NETWORK_JOIN_REQUEST;
			break;
		case APP_NETWORK_JOIN_REQUEST:
			network_start();
			appState = APP_NETWORK_JOINING_STATE;
		case APP_NETWORK_JOINING_STATE:
			break;
		case APP_NETWORK_LEAVING_STATE:
			break;
		case APP_NETWORK_JOINED_STATE:
			break;
		default:
			break;
	}
	SYS_PostTask(APL_TASK_ID);
}

int main(void)
{ 
	SYS_SysInit();
	for(;;)
	{
 		SYS_RunTask();
	}
}


