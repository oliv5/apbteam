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
#include "servo.h"
#include "codewheel.h"

laser_s laser;

/* This function initializes the laser pin input and associated interrupt */
void laser_init(void)
{	
	/* Init laser structiure */
	laser_set_angle(0);
	
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


/* This function returns the angle value in raw format */
uint16_t laser_get_angle_raw(void)
{
	uprintf("valeur de laser_get_angle_raw() = %d\r\n",laser.angle);
	return laser.angle;
}

/* This function returns the angle value in degree */
float laser_get_angle_degree(void)
{
	return codewheel_convert_angle_raw2degrees(laser.angle);
}


/* This function sets the angle value in raw format */
void laser_set_angle(uint16_t angle)
{
	laser.angle = angle;
}


/* Zigbee sending IRQ vector */
ISR(TIMER3_COMPB_vect)
{
	if(servo_get_state(SERVO_1) == SERVO_SCANNING_FAST_IN_PROGRESS)
	{
		codewheel_set_rebase_offset(laser_get_angle_raw());
		codewheel_set_state(CODEWHEEL_REQUEST_REBASE);
	}
	
	/* Disable the interrupt */
	laser_inhibit_angle_confirmation();
}


/* Laser IRQ vector */
ISR(TIMER3_CAPT_vect)
{
	static uint16_t virtual_angle = 0;
	TLaser_edge_type current_edge;
	
	/* Check which kind of edge triggered the interrupt */
	current_edge = laser_get_edge_type();

	/* Could be a bounce so inhibit the latest angle confirmation */
	laser_inhibit_angle_confirmation();
	
	switch(current_edge)
	{
		/* First rising edge of a reflector */
		case LASER_FIRST_RISING_EDGE:
			virtual_angle = ICR3;
			break;
			
		/* Common rising edge of a reflector  */
		case LASER_RISING_EDGE:
			
			/* Recompute the angle value */
			virtual_angle = (virtual_angle + ICR3) / 2;
			break;
			
		/* Falling edge detected */
		case LASER_FALLING_EDGE:
			
			/* Recompute the angle value */
			virtual_angle = (virtual_angle + ICR3) / 2;
			
			/* It's a falling edge so potentially current_angle could be a real one */
			laser_set_angle(virtual_angle);
			
			/* Start virtual angle confirmation */
			laser_engage_angle_confirmation(ICR3);
			break;
			
		default:
			break;
	}
	
	/* Invert the edge detection to catch next rising or falling edge */
	laser_invert_IRQ_edge_trigger();
}
