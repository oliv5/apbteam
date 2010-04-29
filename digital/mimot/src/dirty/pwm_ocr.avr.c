/* pwm_ocr.avr.c - PWM using internal generator. */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2008 Nicolas Schodet
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
#include "common.h"
#include "pwm_ocr.avr.h"
#include "pwm.h"

#include "modules/utils/utils.h"
#include "io.h"

/** Assign PWM outputs. */
#define PWM1 pwm_aux0
#define PWM2 pwm_aux1

#define PWM1_OCR OCR1A
#define PWM1_OCR_BIT 5
#define PWM1_DIR 3
#define PWM2_OCR OCR1B
#define PWM2_OCR_BIT 4
#define PWM2_DIR 2

#define PWM1_BRK_IO A, 4
#define PWM2_BRK_IO A, 5

#include "pwm_config.h"

/** PWM reverse direction, port D. */
static uint8_t pwm_ocr_dir_reverse_d;

/** Initialise PWM generator. */
void
pwm_ocr_init (void)
{
    /* Phase Correct PWM, TOP = 0x3ff, OCnA & OCnB with positive logic.
     * f_IO without prescaler.
     * Fpwm = f_IO / (2 * prescaler * (1 + TOP)) = 7200 Hz. */
#if PWM1or2
    TCCR1A =
	regv (COM1A1, COM1A0, COM1B1, COM1B0, FOC1A, FOC1B, WGM11, WGM10,
		   1,      0,      1,      0,     0,     0,     1,     1);
    TCCR1B = regv (ICNC1, ICES1, 5, WGM13, WGM12, CS12, CS11, CS10,
		       0,     0, 0,     0,     0,    0,    0,    1);
    /* Enable PWM and direction outputs in DDRD. */
    DDRD |= PWM1c (_BV (PWM1_OCR_BIT) | _BV (PWM1_DIR))
	| PWM2c (_BV (PWM2_OCR_BIT) | _BV (PWM2_DIR));
    /* Will activate output at first non zero PWM. */
# ifdef PWM1
    IO_OUTPUT (PWM1_BRK_IO);
# endif
# ifdef PWM2
    IO_OUTPUT (PWM2_BRK_IO);
# endif
#endif
}

/** Update the hardware PWM values. */
void
pwm_ocr_update (void)
{
#if PWM1or2
    uint8_t dir_d;
    /* Sample port D. */
    dir_d = PORTD & ~(PWM1c (_BV (PWM1_DIR)) | PWM2c (_BV (PWM2_DIR)));
# ifdef PWM1
    uint16_t pwm1;
    /* Set PWM1. */
    if (PWM_VALUE (PWM1) == 0)
      {
	pwm1 = 0;
      }
    else
      {
	IO_SET (PWM1_BRK_IO);
	if (PWM_VALUE (PWM1) < 0)
	  {
	    pwm1 = -PWM_VALUE (PWM1);
	  }
	else
	  {
	    dir_d |= _BV (PWM1_DIR);
	    pwm1 = PWM_VALUE (PWM1);
	  }
      }
# endif /* PWM1 */
# ifdef PWM2
    uint16_t pwm2;
    /* Set PWM2. */
    if (PWM_VALUE (PWM2) == 0)
      {
	pwm2 = 0;
      }
    else
      {
	IO_SET (PWM2_BRK_IO);
	if (PWM_VALUE (PWM2) < 0)
	  {
	    pwm2 = -PWM_VALUE (PWM2);
	  }
	else
	  {
	    dir_d |= _BV (PWM2_DIR);
	    pwm2 = PWM_VALUE (PWM2);
	  }
      }
# endif /* PWM2 */
#endif /* PWM1or2 */
    /* Setup registers. */
    /* Here, there could be a problem because OCRx are double buffered, not
     * PORTx! */
    /* Another problem arise if the OCR sampling is done between left and
     * right OCR: the right PWM is one cycle late. */
    /* A solution could be to use interrupts to update PWM or to synchronise
     * general timer with PWM. */
#if PWM1or2
    dir_d ^= pwm_ocr_dir_reverse_d;
    PORTD = dir_d;
# ifdef PWM1
    PWM1_OCR = pwm1;
# endif
# ifdef PWM2
    PWM2_OCR = pwm2;
# endif
#endif /* PWM1or2 */
}

void
pwm_ocr_set_reverse (uint8_t reverse)
{
    pwm_reverse = reverse;
    pwm_ocr_dir_reverse_d = 0;
#ifdef PWM1
    if (reverse & PWM_REVERSE_BIT (PWM1))
	pwm_ocr_dir_reverse_d |= _BV (PWM1_DIR);
#endif
#ifdef PWM2
    if (reverse & PWM_REVERSE_BIT (PWM2))
	pwm_ocr_dir_reverse_d |= _BV (PWM2_DIR);
#endif
}

