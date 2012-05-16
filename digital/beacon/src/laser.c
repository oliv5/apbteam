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
	OCR3B = (value + LASER_CONFIRMATION_OFFSET)%CODEWHEEL_CPR;
	
	/* Enable interrupt */
	TIMSK3 |= (1<<OCIE3B);
	sei(); 
}


/* This function returns the angle value in raw format */
uint16_t laser_get_angle_raw(void)
{
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
	if(calibration_get_state() != SCANNING_STATE_CALIBRATED)
	{
		if(codewheel_get_state() == CODEWHEEL_INIT)
		{
			codewheel_set_rebase_offset(laser_get_angle_raw());
			codewheel_set_state(CODEWHEEL_REQUEST_REBASE);
		}
		else
		{
			/* If mire 1 is spotted */
			if(((laser_get_angle_degree() <= SERVO_1_ANGLE_POSITION + SERVO_ANGLE_POSITION_TOLERANCE) || (laser_get_angle_degree() >= 360 - SERVO_ANGLE_POSITION_TOLERANCE)) && ((servo_get_state(SERVO_1) == SERVO_SCANNING_FAST_IN_PROGRESS) || (servo_get_state(SERVO_1) == SERVO_SCANNING_SLOW_IN_PROGRESS)))
			{
				calibration_set_laser_flag(SET_SERVO_1);
			}
			/* If mire 2 is spotted */
			else if(((laser_get_angle_degree() <= SERVO_2_ANGLE_POSITION + SERVO_ANGLE_POSITION_TOLERANCE) && (laser_get_angle_degree() >= SERVO_2_ANGLE_POSITION - SERVO_ANGLE_POSITION_TOLERANCE)) && ((servo_get_state(SERVO_2) == SERVO_SCANNING_FAST_IN_PROGRESS) || (servo_get_state(SERVO_2) == SERVO_SCANNING_SLOW_IN_PROGRESS)))
			{
				calibration_set_laser_flag(SET_SERVO_2);		
			}
		}
	}
	else
	{
		// TODO: Send  angle
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
			/* UseI ICR3 for now*/
			virtual_angle = ICR3;
			
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
