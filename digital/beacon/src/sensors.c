/* sensors.c */
/* Beacon sensors management. {{{
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
#include <avr/interrupt.h>
#include "sensors.h"
#include "debug.h"
#include "led.h"

/********************************************************/

/* This function initializes the laser pin input and associated interrupt */
void sensors_laser_init(void)
{	
	/* Configure Input compare interrupts for Laser Interrupt*/
	TCCR3B |= (1<<ICNC3)|(1<<ICES3);
	TIMSK3 |= (1<<ICIE3);
	sei(); 
}


/* This function initializes the codewheel optical sensors and associated interrupt */
void sensors_codewheel_init(void)
{	
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


/* This function returns the wheel position */
uint16_t sensors_codewheel_get_value(void)
{
	return TCNT3;
}

/* This function resets the wheel position */
void sensors_codewheel_reset(void)
{
	TCNT3 = 0;
}

/* IRQ vector for CodeWheel complete turn */
ISR(TIMER3_COMPA_vect)
{
	//Top tour ++
}

/* IRQ vector for Laser Interrupt */
ISR(TIMER3_CAPT_vect)
{
	// LASER
}
