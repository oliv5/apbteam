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
#include "twi_specific.h"
#include "calibration.h"
#include "codewheel.h"
#include "print.h"
#include "network.h"
#include "position.h"
#include "buttons.h"
#include "motor.h"
#include "laser.h"
#include "servo.h"
#include "led.h"
#include "twi.h"

static uint8_t uid;

void APL_TaskHandler(void)
{
	switch (network_get_state())
	{
		case APP_INITIAL_STATE:
			
			/* Init Led */
			init_led();
			
			/* Init Serial Interface for debug */ 
  			initSerialInterface();          
			
			uid = get_uid();
			
			/* Init network */
			network_init(uid);
			
			if(get_device_type(uid) == DEVICE_TYPE_COORDINATOR)
			{
				position_init_struct();
				twi_init_specific();
// 				trust_decrease_task();
				start_codewheel_timer_task();
			}
			else
			{
				buttons_init();
				servo_init();
				codewheel_init();
				laser_init();
				motor_init();
				calibration_start_task();
				calibration_start_manual_task();
				motor_stop();
			}
 			network_set_state(APP_NETWORK_JOIN_REQUEST);
			break;
		case APP_NETWORK_JOIN_REQUEST:
			
			/* Activate the network status led blink */
			led_start_blink();
			
			/* St	art network */
			network_start();
			
			network_set_state(APP_NETWORK_JOINING_STATE);
			
		case APP_NETWORK_JOINING_STATE:
			break;
		case APP_NETWORK_LEAVING_STATE:
			break;
		case APP_NETWORK_JOINED_STATE:
			led_stop_blink();
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


