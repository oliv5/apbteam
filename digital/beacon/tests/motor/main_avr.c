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
#include <appTimer.h>
#include "configuration.h"
#include "motor.h"
#include "print.h"
#include "led.h"

uint8_t motor_value = 0;
uint8_t motor_sense = 1;
static HAL_AppTimer_t motorTimer;						// TIMER descripor used by the DEBUG task


void motor_task(void)
{
	if(motor_value == MOTOR_SPEED_MAX)
	{
		motor_sense = -1;
		uprintf("up\r\n");
	}
	else if(motor_value == MOTOR_SPEED_MIN)
	{
		uprintf("down\r\n");
		motor_sense = 1;
	}
	
	motor_value = motor_value + motor_sense;
	motor_set_speed(motor_value);
}

void APL_TaskHandler(void)
{
	static int init_done = 0;
	if(init_done == 0)
	{	
		/* Init Led */
		init_led();
		
		/* Init Serial Interface for debug */ 
		initSerialInterface();          	
		
		motor_init();
		
		init_done = 1;
		
		motorTimer.interval = 50L;
		motorTimer.mode     = TIMER_REPEAT_MODE;
		motorTimer.callback = motor_task;
		HAL_StartAppTimer(&motorTimer);
		uprintf("test_motor : init done\r\n");
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


