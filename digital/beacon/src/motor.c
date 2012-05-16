/* motor.c */
/* Motor control. {{{
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

#include "motor.h"

/* This function initializes the motor control output */
void motor_init(void)
{
	/* Select ouptut */
	DDRB |= (1<<PB7);	
	
	OCR0A = 0;
	
	/* Fast PWM 10bits with TOP=0x03FF */
	TCCR0A |= (1<<WGM01)|(1<<WGM00);
	
	/* Prescaler = 1 */
	TCCR0B |= (1<<CS01);
	
	/* Postive Logic */
	TCCR0A |= (1<<COM0A1);

	/* Enable Interrupts */
	sei();
}

/* This function starts the motor rotation */
void motor_start(void)
{
	OCR0A = 115;

}

/* This function stops the motor rotation */
void motor_stop(void)
{
	OCR0A = 0;
}

/* This function returns the motor state */
TMotor_state motor_get_state(void)
{
	if(OCR0A != 0)
		return MOTOR_IN_ROTATION;
	else
		return MOTOR_STOPPED;
}

/* This function starts or stops the motor according to the current state */
void motor_start_stop_control(void)
{
	if(motor_get_state() == MOTOR_IN_ROTATION)
		motor_stop();
	else
		motor_start();
}


ISR(TIMER0_COMPA_vect)
{
}