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

#include <types.h>
#include <avr/interrupt.h>
#include "debug_avr.h"
#include "codewheel.h"

codewheel_s codewheel;

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

/* This function resets the wheel position */
void codewheel_reset(void)
{
	TCNT3 = 0;
}

/* Codewheel complete turn IRQ vector for CodeWheel*/
ISR(TIMER3_COMPA_vect)
{
}
