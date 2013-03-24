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
#include "print.h"
#include "led.h"

static HAL_AppTimer_t bouttonsTimer;						// TIMER descripor used by the DEBUG task

void bouttons_task(void)
{
	if(!(PIND&0x08))
	{
		uprintf("boutton 1\r\n");
		PORTD |= (1<<PORTD0);
	}
	else if(!(PIND&0x04))
	{	
		uprintf("boutton 2\r\n");
		PORTD |= (1<<PORTD1);
	}
	else if(!(PIND&0x02))
	{
		uprintf("boutton 3\r\n");
		PORTD |= (1<<PORTD2);
	}
	else if(!(PIND&0x01))
	{
		uprintf("boutton 4\r\n");
		PORTD |= (1<<PORTD3);
	}
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
		
		
		DDRD = 0x0F;
		PORTD = 0x0F;
		
		bouttonsTimer.interval = 400L;
		bouttonsTimer.mode     = TIMER_REPEAT_MODE;
		bouttonsTimer.callback = bouttons_task;
		HAL_StartAppTimer(&bouttonsTimer);
		
		init_done = 1;
		uprintf("test_bouttons : init done\r\n");
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


