/* laser.c */
/* laser sensor management. {{{
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
#include "laser.h"


/* This function initializes the laser pin input and associated interrupt */
void laser_init(void)
{	
	/* Configure Input compare interrupts for Laser Interrupt*/
	TCCR3B |= (1<<ICNC3)|(1<<ICES3);
	TIMSK3 |= (1<<ICIE3);
	TIMSK3 &= ~(1<<OCIE3B);
	sei(); 
}


/* This function returns the edge type trigged by the AVR IC module*/
TLaser_edge_type laser_get_edge_type(void)
{
	if(RISING_EDGE)
	{
		if(SENDING_ENGAGED)
		{
			return LASER_RISING_EDGE;
		}
		else
		{
			return LASER_FIRST_RISING_EDGE;
		}
	}
	else
	{
		return LASER_FALLING_EDGE;
	}
}


/* This function inverts the trigged edge of the AVR IC module */
void laser_invert_IRQ_edge_trigger(void)
{
	TCCR3B ^= 1<<ICES3;
}


/* This function deactivates the angle sending */
void laser_inhibit_angle_confirmation(void)
{
	TIMSK3 &= ~(1<<OCIE3B);
	sei(); 
}


/* This function configures the AVR OC3B interrupt that will send the angle LASER_SENDING_OFFSET after the latest rising edge */
void laser_engage_angle_confirmation(uint16_t value)
{
	OCR3A = value + LASER_CONFIRMATION_OFFSET;
	TIMSK3 |= (1<<OCIE3B);
	sei(); 
}


/* Zigbee sending IRQ vector */
ISR(TIMER3_COMPB_vect)
{
// 	TIMSK3 &= ~OCIE3B;
// 	sei(); 
}


/* Laser IRQ vector */
ISR(TIMER3_CAPT_vect)
{
	static uint16_t angle = 0;
	TLaser_edge_type current_edge;
	
	current_edge = laser_get_edge_type();
	
	switch(current_edge)
	{
		case LASER_FIRST_RISING_EDGE:
			angle = ICR3;
			break;
		case LASER_RISING_EDGE:
			angle = (angle + ICR3) / 2;
			/* Could be a bounce so inhibit the latest angle confirmation */
			laser_inhibit_angle_confirmation();
			break;
		case LASER_FALLING_EDGE:
			angle = (angle + ICR3) / 2;
			/* Start virtual angle confirmation */
			laser_engage_angle_confirmation(ICR3);
			break;
		default:
			break;
	}
	laser_invert_IRQ_edge_trigger();
}
