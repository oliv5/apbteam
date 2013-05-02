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

#include "print.h"
#include "motor.h"
#include "codewheel.h"

motor_s motor;

/* This function initializes the motor control output */
void motor_init(void)
{
	/* Select ouptut */
	DDRB |= (1<<PB7);	
	
	OCR0A = MOTOR_SPEED_MIN;
	
	/* Fast PWM 10bits with TOP=0x03FF */
	TCCR0A |= (1<<WGM01)|(1<<WGM00);
	
	/* Prescaler = 1 */
	TCCR0B |= (1<<CS01);
	
	/* Postive Logic */
	TCCR0A |= (1<<COM0A1);

	motor_set_target_speed(MOTOR_TARGET_SPEED_INIT);
	/* Enable Interrupts */
	sei();
}

/* This function starts the motor rotation */
void motor_start(void)
{
	motor_set_speed(MOTOR_SPEED_MAX);
	start_codewheel_timer_task();
}

/* This function stops the motor rotation */
void motor_stop(void)
{
	motor_set_speed(MOTOR_SPEED_STOP);
	motor_set_target_speed(MOTOR_SPEED_STOP);
	stop_codewheel_timer_task();
}


/* This function sets the motor speed */
void motor_set_speed(uint8_t value)
{
	if(value >= MOTOR_SPEED_MAX)
		OCR0A = MOTOR_SPEED_MAX;
	else if(value <= MOTOR_SPEED_STOP)
		OCR0A = MOTOR_SPEED_STOP;
	else
		OCR0A = value;
}

/* This function returns the motor speed in raw format */
uint8_t motor_get_speed_raw()
{
	return OCR0A;
}

/* This function returns the motor state */
TMotor_state motor_get_state(void)
{
	if(OCR0A > MOTOR_SPEED_MIN)
		return MOTOR_IN_ROTATION;
	else
		return MOTOR_STOPPED;
}

/* This function starts or stops the motor according to the current state */
void motor_start_stop_control(void)
{
	if(motor_get_state() == MOTOR_IN_ROTATION)
	{
		motor_stop();
	}
	else
	{
		motor_start();
	}
}

/* This function sets the target speed */
void motor_set_target_speed(uint8_t value)
{
	motor.target_speed = value;
}

/* This function returns the target speed */
uint8_t motor_get_target_speed()
{
	return motor.target_speed;
}

/* This function control the motor speed accroding to target speed requested */
void motor_control_speed(uint16_t time)
{
	int16_t diff = 0;
	int16_t correction = 0;
	
 	diff = motor_get_target_speed() - time;

	if(diff > 15)
		correction = motor_get_speed_raw() - 5;
	else if (diff < -15)
		correction = motor_get_speed_raw() + 5;
	else
		correction = motor_get_speed_raw();
	
	motor_set_speed(correction);

}

ISR(TIMER0_COMPA_vect)
{
}