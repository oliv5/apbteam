/* servo.c */
/* Beacon servomotor management. {{{
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
#include "servo.h" 
#include "debug.h"

/* This function initializes the timer used for servomotor signal generation */
void servo_timer1_init(void)
{

	//Fpwm = f_IO / (prescaler * (1 + TOP)) = 7200 Hz. */
  	OCR1B = SERVO_1_ANGLE_INIT; 
  	OCR1A = SERVO_2_ANGLE_INIT;

	
	/* Fast PWM 10bits with TOP=0x03FF */
	TCCR1A |= (1<<WGM11)|(1<<WGM10);
	TCCR1B |= (1<<WGM12);
	
	/* Prescaler = 256 */
	TCCR1B |= (1<<CS11) | (1<<CS10);
	
// 	/* Prescaler = 1 */
// 	TCCR1B |= (1<<CS10);

	/* Postive Logic */
 	TCCR1A |= (1<<COM1A1)|(1<<COM1B1);
	
	/* Select ouptut */
 	DDRB |= (1<<PB5) | (1<<PB6);
	
	
	/* Configure Overflow and Input compare interrupts */
   	TIMSK1 |= (1<<TOIE1); 

	/* Enable Interrupts */
 	sei(); 

}

/* This function increase by one unit the angle of the defined servo and returns "angle" value */
int servo_angle_increase(TServo_ID servo_id)
{
	switch(servo_id)
	{
		case SERVO_1:
			if(OCR1A < SERVO_1_ANGLE_MAX)
			{
				OCR1A++;
				return OCR1A;
			}
			break;
		case SERVO_2:
			if(OCR1B < SERVO_2_ANGLE_MAX)
			{
				OCR1B++;
				return OCR1B;
			}
			break;
		default:
			break;
	}
	return 0;
}


/* This function decrease by one unit the angle of the defined servo and returns "angle" value*/
int servo_angle_decrease(TServo_ID servo_id)
{
	switch(servo_id)
	{
		case SERVO_1:
			if(OCR1A > SERVO_1_ANGLE_MIN)
			{
				OCR1A--;
				return OCR1A;
			}
			break;
		case SERVO_2:
			if(OCR1B > SERVO_2_ANGLE_MIN)
			{
				OCR1B--;
				return OCR1B;
			}
			break;
		default:
			break;
	}
	return 0;
}

SIGNAL (SIG_OVERFLOW1)
{
}

