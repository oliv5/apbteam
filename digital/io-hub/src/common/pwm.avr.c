/* pwm.avr.c */
/* io-hub - Modular Input/Output. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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
#include "pwm.h"

#include "modules/utils/utils.h"
#include "preproc.h"

/** PWM context. */
struct pwm_t
{
    /** Output compare register. */
    volatile uint16_t *ocr;
    /** Direction port. */
    volatile uint8_t *dir_port;
    /** Direction bit value. */
    uint8_t dir_bv;
    /** Remaining time before timer elapse, 0 for disabled. */
    uint16_t time;
    /** Value to be used after timer elapse. */
    int16_t value_rest;
};

/** Array of PWM contexts. */
#define PWM(timer, pwm, pwm_io_port, pwm_io_n, dir_io_port, dir_io_n) \
{ &PREPROC_PASTE (OCR, timer, pwm), \
    &IO_PORT_ (dir_io_port, dir_io_n), IO_BV_ (dir_io_port, dir_io_n), \
    0, 0 },
struct pwm_t pwm[] = {
    AC_IOHUB_PWM
};
#undef PWM

void
pwm_init (void)
{
    /* Fast PWM, TOP = 0x3ff, with positive logic.
       f_IO without prescaler.
       Fpwm = f_IO / (prescaler * (1 + TOP)). */
    uint8_t timer_com_1 = 0, timer_com_3 = 0;
#define PWM(timer, pwm, pwm_io_port, pwm_io_n, dir_io_port, dir_io_n) \
    do { \
	PREPROC_PASTE (timer_com_, timer) |= \
	    _BV (PREPROC_PASTE (COM, timer, pwm, 1)); \
	IO_DDR_ (pwm_io_port, pwm_io_n) |= IO_BV_ (pwm_io_port, pwm_io_n); \
	IO_DDR_ (dir_io_port, dir_io_n) |= IO_BV_ (dir_io_port, dir_io_n); \
    } while (0);
    AC_IOHUB_PWM
#undef PWM
    if (timer_com_1)
      {
	TCCR1A = timer_com_1 |
	    regv (COM1A1, COM1A0, COM1B1, COM1B0, COM1C1, COM1C0, WGM11, WGM10,
		       0,      0,      0,      0,      0,      0,     1,     1);
	TCCR1B = regv (ICNC1, ICES1, 5, WGM13, WGM12, CS12, CS11, CS10,
			   0,     0, 0,     0,     1,    0,    0,    1);
      }
    if (timer_com_3)
      {
	TCCR3A = timer_com_3 |
	    regv (COM3A1, COM3A0, COM3B1, COM3B0, COM3C1, COM3C0, WGM31, WGM30,
		       0,      0,      0,      0,      0,      0,     1,     1);
	TCCR3B = regv (ICNC3, ICES3, 5, WGM33, WGM32, CS32, CS31, CS30,
			   0,     0, 0,     0,     1,    0,    0,    1);
      }
}

void
pwm_update (void)
{
    uint8_t i;
    for (i = 0; i < UTILS_COUNT (pwm); i++)
      {
	if (pwm[i].time)
	  {
	    pwm[i].time--;
	    if (pwm[i].time == 0)
		pwm_set (i, pwm[i].value_rest);
	  }
      }
}

void
pwm_set (uint8_t index, int16_t value)
{
    assert (index < UTILS_COUNT (pwm));
    assert (value <= PWM_MAX && value >= -PWM_MAX);
    pwm[index].time = 0;
    if (value >= 0)
      {
	*pwm[index].dir_port &= ~pwm[index].dir_bv;
	*pwm[index].ocr = value;
      }
    else
      {
	*pwm[index].dir_port |= pwm[index].dir_bv;
	*pwm[index].ocr = -value;
      }
}

void
pwm_set_timed (uint8_t index, int16_t value, uint16_t time, int16_t value_rest)
{
    assert (index < UTILS_COUNT (pwm));
    assert (value <= PWM_MAX && value >= -PWM_MAX);
    assert (value_rest <= PWM_MAX && value_rest >= -PWM_MAX);
    pwm_set (index, value);
    pwm[index].time = time;
    pwm[index].value_rest = value_rest;
}

