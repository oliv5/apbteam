/* codewheel.c */
/* Codewheel sensors management. {{{
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

#include <avr/interrupt.h>
#include <types.h>
#include <math.h>
#include "debug_avr.h"
#include "codewheel.h"
#include "laser.h"
#include "network.h"

codewheel_s codewheel;
static HAL_AppTimer_t codewheelTimer;

/* This function initializes the codewheel optical sensors and associated interrupt */
void codewheel_init(void)
{	
	/* Set codewheel state*/
	codewheel_set_state(CODEWHEEL_INIT);
	
	/* Select external clock on rising edge for timer 3 */
	TCCR3B |= (1<<CS30)|(1<<CS31)|(1<<CS32);
	
	/* Use CTC mode */
	TCCR3B |= (1<<WGM32);
	
	/* Define the compare value */
	OCR3A = CODEWHEEL_CPR;
	
	/* Enable Interrupts */
	TIMSK3 |= (1<<OCIE3A);
	sei(); 
}

/* This function resets the wheel position */
void codewheel_reset(void)
{
	TCNT3 = 0;
}

/* This function returns the codewheel state */
TCodewheel_state codewheel_get_state(void)
{
	return codewheel.state;
}

/* This function modify the codewheel state */
void codewheel_set_state(TCodewheel_state state)
{
	codewheel.state = state;
}

/* This function returns the wheel position */
uint16_t codewheel_get_value(void)
{
	return TCNT3;
}

/* This function returns the offset value */
uint16_t codewheel_get_rebase_offset(void)
{
	return codewheel.rebase_offset;
}

/* This function saves the counter value used when a codewheel reset is requested */
void codewheel_set_rebase_offset(uint16_t offset)
{
	codewheel.rebase_offset = offset;
}

/* This function converts the angle value from row format to degrees */
float codewheel_convert_angle_raw2degrees(uint16_t raw_value)
{
	return  (float)raw_value*(float)360/(float)CODEWHEEL_CPR;
}

/* This function converts the angle value from row format to radians */
float codewheel_convert_angle_raw2radians(uint16_t raw_value)
{
	return  (float)raw_value*(float)(2*M_PI)/(float)CODEWHEEL_CPR;
}

/* Codewheel complete turn IRQ vector for CodeWheel*/
ISR(TIMER3_COMPA_vect)
{
	if(codewheel_get_state() == CODEWHEEL_REQUEST_REBASE)
	{
		OCR3A = codewheel_get_rebase_offset();
		codewheel_set_state(CODEWHEEL_REBASED);
		uprintf("Rebased\r\n");
	}	
	else
	{
		OCR3A = CODEWHEEL_CPR;
		codewheel.time = 0;
	}
	laser_reset_angle_id();
}

/* Task for turn time measurment */
void codewheel_timer_task(void)
{
	codewheel.time+=10;
}

/* This function start the codewheel timer task */
void start_codewheel_timer_task(void)
{
	codewheel.time = 0;
	codewheelTimer.interval = CODEWHEEL_TIMER_TASK_PERIOD;
	codewheelTimer.mode     = TIMER_REPEAT_MODE;
	codewheelTimer.callback = codewheel_timer_task;
	HAL_StartAppTimer(&codewheelTimer);
}

/* This function stop the codewheel timer task */
void stop_codewheel_timer_task(void)
{
	HAL_StopAppTimer(&codewheelTimer);
}


