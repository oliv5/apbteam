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
#undef PWM1
#undef PWM2
#undef PWM3
#undef PWM4

#define PWM1_OCR OCR1B
#define PWM1_OCR_BIT 6
#define PWM1_DIR 4
#define PWM2_OCR OCR1C
#define PWM2_OCR_BIT 7
#define PWM2_DIR 5
#define PWM3_OCR OCR3B
#define PWM3_OCR_BIT 4
#define PWM3_DIR 2
#define PWM4_OCR OCR3C
#define PWM4_OCR_BIT 5
#define PWM4_DIR 3

#include "pwm_config.h"

/** PWM reverse direction, port B. */
static uint8_t pwm_ocr_dir_reverse_b;
/** PWM reverse direction, port E. */
static uint8_t pwm_ocr_dir_reverse_e;

/** Initialise PWM generator. */
void
pwm_ocr_init (void)
{
    /* Fast PWM, TOP = 0x3ff, OCnB & OCnC with positive logic.
       f_IO without prescaler.
       Fpwm = f_IO / (prescaler * (1 + TOP)) = 14400 Hz. */
#if PWM1or2
    TCCR1A =
	regv (COM1A1, COM1A0, COM1B1, COM1B0, COM1C1, COM1C0, WGM11, WGM10,
		   0,      0,      1,      0,      1,      0,     1,     1);
    TCCR1B = regv (ICNC1, ICES1, 5, WGM13, WGM12, CS12, CS11, CS10,
		       0,     0, 0,     0,     1,    0,    0,    1);
    /* Enable pwm and direction outputs in DDRB. */
    DDRB |= PWM1c (_BV (PWM1_OCR_BIT) | _BV (PWM1_DIR))
	| PWM2c (_BV (PWM2_OCR_BIT) | _BV (PWM2_DIR));
#endif
#if PWM3or4
    TCCR3A =
	regv (COM3A1, COM3A0, COM3B1, COM3B0, COM3C1, COM3C0, WGM31, WGM30,
		   0,      0,      1,      0,      1,      0,     1,     1);
    TCCR3B = regv (ICNC3, ICES3, 5, WGM33, WGM32, CS32, CS31, CS30,
		       0,     0, 0,     0,     1,    0,    0,    1);
    /* Enable pwm and direction outputs in DDRE. */
    DDRE |= PWM3c (_BV (PWM3_OCR_BIT) | _BV (PWM3_DIR))
	| PWM4c (_BV (PWM4_OCR_BIT) | _BV (PWM4_DIR));
#endif
}

/** Update the hardware PWM values. */
void
pwm_ocr_update (void)
{
#if PWM1or2
    uint8_t dir_b;
    /* Sample port B. */
    dir_b = PORTB & ~(PWM1c (_BV (PWM1_DIR)) | PWM2c (_BV (PWM2_DIR)));
# ifdef PWM1
    uint16_t pwm1;
    /* Set PWM1. */
    if (PWM_VALUE (PWM1) == 0)
      {
	pwm1 = 0;
      }
    else if (PWM_VALUE (PWM1) < 0)
      {
	pwm1 = -PWM_VALUE (PWM1);
      }
    else
      {
	dir_b |= _BV (PWM1_DIR);
	pwm1 = PWM_VALUE (PWM1);
      }
# endif /* PWM1 */
# ifdef PWM2
    uint16_t pwm2;
    /* Set PWM2. */
    if (PWM_VALUE (PWM2) == 0)
      {
	pwm2 = 0;
      }
    else if (PWM_VALUE (PWM2) < 0)
      {
	pwm2 = -PWM_VALUE (PWM2);
      }
    else
      {
	dir_b |= _BV (PWM2_DIR);
	pwm2 = PWM_VALUE (PWM2);
      }
# endif /* PWM2 */
#endif /* PWM1or2 */
#if PWM3or4
    uint8_t dir_e;
    /* Sample port E. */
    dir_e = PORTE & ~(PWM3c (_BV (PWM3_DIR)) | PWM4c (_BV (PWM4_DIR)));
# ifdef PWM3
    uint16_t pwm3;
    /* Set PWM3. */
    if (PWM_VALUE (PWM3) == 0)
      {
	pwm3 = 0;
      }
    else if (PWM_VALUE (PWM3) < 0)
      {
	pwm3 = -PWM_VALUE (PWM3);
      }
    else
      {
	dir_e |= _BV (PWM3_DIR);
	pwm3 = PWM_VALUE (PWM3);
      }
# endif /* PWM3 */
# ifdef PWM4
    uint16_t pwm4;
    /* Set PWM4. */
    if (PWM_VALUE (PWM4) == 0)
      {
	pwm4 = 0;
      }
    else if (PWM_VALUE (PWM4) < 0)
      {
	pwm4 = -PWM_VALUE (PWM4);
      }
    else
      {
	dir_e |= _BV (PWM4_DIR);
	pwm4 = PWM_VALUE (PWM4);
      }
# endif /* PWM4 */
#endif /* PWM3or4 */
    /* Setup registers. */
    /* Here, there could be a problem because OCRx are double buffered, not
     * PORTx! */
    /* Another problem arise if the OCR sampling is done between left and
     * right OCR: the right PWM is one cycle late. */
    /* A solution could be to use interrupts to update PWM or to synchronise
     * general timer with PWM. */
#if PWM1or2
    dir_b ^= pwm_ocr_dir_reverse_b;
    PORTB = dir_b;
# ifdef PWM1
    PWM1_OCR = pwm1;
# endif
# ifdef PWM2
    PWM2_OCR = pwm2;
# endif
#endif /* PWM1or2 */
#if PWM3or4
    dir_e ^= pwm_ocr_dir_reverse_e;
    PORTE = dir_e;
# ifdef PWM3
    PWM3_OCR = pwm3;
# endif
# ifdef PWM4
    PWM4_OCR = pwm4;
# endif
#endif /* PWM3or4 */
}

void
pwm_ocr_set_reverse (uint8_t reverse)
{
    pwm_reverse = reverse;
    pwm_ocr_dir_reverse_b = 0;
    pwm_ocr_dir_reverse_e = 0;
#ifdef PWM1
    if (reverse & PWM_REVERSE_BIT (PWM1))
	pwm_ocr_dir_reverse_b |= _BV (PWM1_DIR);
#endif
#ifdef PWM2
    if (reverse & PWM_REVERSE_BIT (PWM2))
	pwm_ocr_dir_reverse_b |= _BV (PWM2_DIR);
#endif
#ifdef PWM3
    if (reverse & PWM_REVERSE_BIT (PWM3))
	pwm_ocr_dir_reverse_e |= _BV (PWM3_DIR);
#endif
#ifdef PWM4
    if (reverse & PWM_REVERSE_BIT (PWM4))
	pwm_ocr_dir_reverse_e |= _BV (PWM4_DIR);
#endif
}

