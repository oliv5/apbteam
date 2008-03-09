/* pwm.avr.c */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2006.
 *        Web: http://assos.efrei.fr/robot/
 *      Email: robot AT efrei DOT fr
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
#include "common.h"
#include "pwm.h"

#include "modules/utils/utils.h"
#include "io.h"

/** Define the PWM output used for left motor. */
#define PWM_LEFT_OCR OCR1B
/** Define the PWM output used for right motor. */
#define PWM_RIGHT_OCR OCR1C
/** Define the direction output for left motor. */
#define PWM_LEFT_DIR 4
/** Define the direction output for right motor. */
#define PWM_RIGHT_DIR 5

/** PWM values, this is an error if absolute value is greater than the
 * maximum. */
int16_t pwm_left, pwm_right;
/** PWM reverse direction, only set pwm dir bits or you will get weird results
 * on port B. */
uint8_t pwm_dir = _BV (PWM_LEFT_DIR);

/** Initialise PWM generator. */
void
pwm_init (void)
{
    /* Fast PWM, TOP = 0x3ff, OC1B & OC1C with positive logic.
       f_IO without prescaler.
       Fpwm = f_IO / (prescaler * (1 + TOP)) = 14400 Hz. */
    TCCR1A =
	regv (COM1A1, COM1A0, COM1B1, COM1B0, COM1C1, COM1C0, WGM11, WGM10,
		   0,      0,      1,      0,      1,      0,     1,     1);
    TCCR1B = regv (ICNC1, ICES1, 5, WGM13, WGM12, CS12, CS11, CS10,
		       0,     0, 0,     0,     1,    0,    0,    1);
    /* Enable pwm and direction outputs in DDRB. */
    DDRB |= _BV (7) | _BV (6) | _BV (PWM_LEFT_DIR) | _BV (PWM_RIGHT_DIR);
}

/** Update the hardware PWM values. */
void
pwm_update (void)
{
    uint16_t left, right;
    uint8_t dir;
    /* Some assumption checks. */
    assert (pwm_left >= -PWM_MAX && pwm_left <= PWM_MAX);
    assert (pwm_right >= -PWM_MAX && pwm_right <= PWM_MAX);
    assert ((pwm_dir & ~(_BV (PWM_LEFT_DIR) | _BV (PWM_RIGHT_DIR))) == 0);
    /* Sample port B. */
    dir = PORTB & ~(_BV (PWM_LEFT_DIR) | _BV (PWM_RIGHT_DIR));
    /* Set left PWM. */
    if (pwm_left == 0)
      {
	left = 0;
      }
    else if (pwm_left < 0)
      {
	left = -pwm_left;
      }
    else
      {
	dir |= _BV (PWM_LEFT_DIR);
	left = pwm_left;
      }
    /* Set right PWM. */
    if (pwm_right == 0)
      {
	right = 0;
      }
    else if (pwm_right < 0)
      {
	right = -pwm_right;
      }
    else
      {
	dir |= _BV (PWM_RIGHT_DIR);
	right = pwm_right;
      }
    /* Setup registers. */
    /* Here, there could be a problem because OCRx are double buffered, not
     * PORTB! */
    /* Another problem arise if the OCR sampling is done between left and
     * right OCR: the right PWM is one cycle late. */
    /* A solution could be to use interrupts to update PWM or to synchronise
     * general timer with PWM. */
    dir ^= pwm_dir;
    PORTB = dir;
    PWM_LEFT_OCR = left;
    PWM_RIGHT_OCR = right;
}

void
pwm_reverse (uint8_t left, uint8_t right)
{
    pwm_dir = 0;
    if (left) pwm_dir |= _BV (PWM_LEFT_DIR);
    if (right) pwm_dir |= _BV (PWM_RIGHT_DIR);
}

