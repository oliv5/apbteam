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
#include <irq.h>
#include "sensors.h"



/********************************************************/

/* This function initializes the laser pin input and associated interrupt */
void sensors_laser_init(void)
{
 	HAL_RegisterIrq(IRQ_7,IRQ_RISING_EDGE,sensors_laser_irq_vector);
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
int sensors_codewheel_get_value(void)
{
	return TCNT3;
}


/* IRQ vector for Laser Interrupt */
void sensors_laser_irq_vector(void)
{
	//calculate and send value
}

/* IRQ vector for CodeWheel complete turn */
ISR(TIMER3_COMPA_vect)
ISR(TIMER3_OVF_vect)
{
	//Top tour ++
}
