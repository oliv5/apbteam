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


/* This function initializes the timer used for servomotor signal generation */
void SERVO_timer1_init(void)
{

	//Fpwm = f_IO / (prescaler * (1 + TOP)) = 7200 Hz. */
  	OCR1B = 210; 
  	OCR1A = 279;

	
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

/* This function increase by one unit the angle of the defined servo */
void SERVO_angle_increase(int servo_id)
{
	switch(servo_id)
	{
		case 1:
			if(OCR1A < SERVO_1_ANGLE_MAX)
			{
				OCR1A++;
			}
			break;
		case 2:
			if(OCR1B < SERVO_2_ANGLE_MAX)
			{
				OCR1B++;
			}
			break;
		default:
			return;
	}
}


/* This function decrease by one unit the angle of the defined servo */
void SERVO_angle_decrease(int servo_id)
{
	switch(servo_id)
	{
		case 1:
			if(OCR1A > SERVO_1_ANGLE_MIN)
			{
				OCR1A--;
			}
			break;
		case 2:
			if(OCR1B > SERVO_2_ANGLE_MIN)
			{
				OCR1B--;
			}
			break;
		default:
			return;
	}
}

SIGNAL (SIG_OVERFLOW1)
{
}


// #define SERVO_NUMBER 8
// #define SERVO_HIGH_TIME_MIN 0x24
// #define SERVO_HIGH_TIME_MAX 0x88
// #define SERVO_TCNT_TOP 0x00FF
// #define set_bit(port, bit) (port |= _BV(bit))
// 
// #define UTILS_BOUND(v, min, max) \
//     do { \
// 	if ((v) < (min)) \
// 	    (v) = (min); \
// 	else if ((v) > (max)) \
// 	    (v) = (max); \
//     } while (0)
// 
// 
// 
// volatile int8_t servo_updating_id_;
// volatile uint8_t servo_high_time_[SERVO_NUMBER];
// static const uint16_t servo_tic_cycle_ = 8000000 / 64 * 20 / 1000;
// 
// union _utils_byte_access
// {
//     uint8_t v8[4];
//     uint16_t v16[2];
//     uint32_t v32;
// };
// 
// /** Byte packing macro, pack 4 bytes into a double word. */
// extern inline uint32_t
// v8_to_v32 (uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0)
// {
//     union _utils_byte_access ba;
//     ba.v8[0] = b0;
//     ba.v8[1] = b1;
//     ba.v8[2] = b2;
//     ba.v8[3] = b3;
//     return ba.v32;
// }
// 
// extern inline uint8_t
// v16_to_v8 (uint16_t w, int pos)
// {
//     union _utils_byte_access ba;
//     ba.v16[0] = w;
//     return ba.v8[pos];
// }
// 
// 
// 
// SIGNAL (SIG_OVERFLOW2);
// 
// 
// void servo_apbteam_init (void)
// {
// 	/* Set-up all the pins of the servo to out direction */
// 	DDRB |= (1<<PB5) | (1<<PB6);
// 	/* All pins are at low state by default */
// 
// 	/* Set-up the timer/counter 1:
// 	- prescaler 1 => 4.44 ms TOP */
// 	TCCR1B |= (1<<CS11)|(1<<CS10);
//  	TCCR1A |= (1<<WGM10);
// 	
// 
// 	/* The state machine start with the first servo */
// 	servo_updating_id_ = 0;
// 
// 	/* Enable overflow interrupt */
//    	TIMSK1 |= (1<<TOIE1); 
// 
// 	/* By default, servo init disable all servo. */
// 	uint8_t i;
// 	for (i = 0; i < SERVO_NUMBER; i++)
// 	servo_set_high_time (i, 0x25);
// }
// 
// void servo_set_high_time (uint8_t servo, uint8_t high_time)
// {
// // 	DDRD=0xE0;
// 	uint8_t filtered = high_time;
// 	if (filtered != 0)
// 		UTILS_BOUND (filtered, SERVO_HIGH_TIME_MIN, SERVO_HIGH_TIME_MAX);
// 	/* Sanity check */
// 	if (servo < SERVO_NUMBER)
// 		/* Set new desired position (high value time) */
// 		servo_high_time_[servo] = filtered;
// }
// 
// 
// uint8_t servo_get_high_time (uint8_t servo)
// {
//     /* Sanity check */
//     if (servo < SERVO_NUMBER)
// 	return servo_high_time_[servo];
//     return 0;
// }
// 
// SIGNAL (SIG_OVERFLOW1)
// {
// DDRD=0xE0;
// 
//     /* Overflow count (used when we wait in the lower state).
//        -1 is used for the first count where we wait less than a complete
//        overflow */
//     static int8_t servo_overflow_count = -1;
//     /* Time spent by each servo motor at high state during a whole cycle */
//     static uint16_t servo_high_time_cycle = servo_tic_cycle_;
// 
//     /* State machine actions */
//     if (servo_updating_id_ >= 0)
//       {
// 	/* Servos motor high state mode */
// 
// 	/* Set to low state the previous servo motor pin if needed (not for
// 	 * the first one) */
// 	if (servo_updating_id_ != 0)
// 	    PORTB &= ~_BV (servo_updating_id_ - 1);
// 	/* Set to high state the current servo motor pin, unless is zero */
// 	if (servo_high_time_[servo_updating_id_])
// 	    set_bit (PORTB, servo_updating_id_);
// 	/* Plan next timer overflow to the TOP minus the current configuration
// 	 * of the servo motor */
// 	TCNT1 = SERVO_TCNT_TOP - servo_high_time_[servo_updating_id_];
// 	/* Update the time spent at high state by all servo motors for this
// 	 * cycle */
// 	servo_high_time_cycle += servo_high_time_[servo_updating_id_];
// 	/* Update the identifier of the current servo motor (and manage when
// 	 * we are at the last one) */
// 	if (++servo_updating_id_ == SERVO_NUMBER)
// 	    servo_updating_id_ = -1;
//       }
//     else
//       {
// 
// 	/* Sleeping time mode */
// 
// 	/* Is it the first we are in this mode? */
// 	if (servo_overflow_count == -1)
// 	  {
// 	    /* Set to low state the previous servo motor pin */
// 	    PORTB &= ~_BV (SERVO_NUMBER - 1);
// 	    /* Number of full overflow (from 0 to SERVO_TCNT_TOP) we need to
// 	     * wait (division by SERVO_TCNT_TOP or >> 8) */
// 	    servo_overflow_count = servo_high_time_cycle >> 8;
// 	    /* Restart the counter from remaining TIC that are left and can
// 	     * not be used to make a full overflow */
// 	    TCNT1 = SERVO_TCNT_TOP - v16_to_v8 (servo_high_time_cycle, 0);
// 	  }
// 	else
// 	  {
// 	    /* We just have an overflow, are we at the last one needed? The -1
// 	     * is normal: we do not count the first overflow of the sleeping
// 	     * mode because it is not a full one */
// 	    if (--servo_overflow_count == -1)
// 	      {
// 		/* Restart with first servo motor */
// 		servo_updating_id_ = 0;
// 		/* Re-initialize the counter of time spent by each servo motor
// 		 * at high state */
// 		servo_high_time_cycle = servo_tic_cycle_;
// 	      }
// 	  }
//       }
// }



