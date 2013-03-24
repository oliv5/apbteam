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

uint8_t sense = 1;

static HAL_AppTimer_t testTimer;

void test_task(void)
{

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

		//Fpwm = f_IO / (prescaler * (1 + TOP)) = 7200 Hz. */
		OCR1B = 150; 
		OCR1A = 150;

		
		/* Fast PWM 10bits with TOP=0x03FF */
		TCCR1A |= (1<<WGM11)|(1<<WGM10);
		TCCR1B |= (1<<WGM12);
		
		/* Prescaler = 256 */
		TCCR1B |= (1<<CS11) | (1<<CS10);

		/* Postive Logic */
		TCCR1A |= (1<<COM1A1)|(1<<COM1B1);
		
		/* Select ouptut */
		DDRB |= (1<<PB5) | (1<<PB6);
		
		
		/* Configure Overflow and Input compare interrupts */
		TIMSK1 |= (1<<TOIE1); 

		/* Enable Interrupts */
		sei(); 
		
		testTimer.interval = 1000L;
		testTimer.mode     = TIMER_REPEAT_MODE;
		testTimer.callback = test_task;
		HAL_StartAppTimer(&testTimer);

		init_done = 1;
		uprintf("test_servo : init done\r\n");
	}
	SYS_PostTask(APL_TASK_ID);
}


SIGNAL (SIG_OVERFLOW1)
{
}



int main(void)
{ 
	SYS_SysInit();
	for(;;)
	{
 		SYS_RunTask();
	}
}


