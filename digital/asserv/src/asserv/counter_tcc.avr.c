/* counter_tcc.avr.c - Internal counter support. */
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
#include "counter.h"

#include "modules/utils/utils.h"
#include "io.h"

#include "misc.h"

/**
 * This file add support for an AVR internal counter.  This uses one TCC
 * hardware per counter, and an external circuitry to compute rotation
 * direction.
 */

/** Define to 1 to reverse the left counter. */
#define COUNTER_LEFT_REVERSE 0
/** Define to 1 to reverse the right counter. */
#define COUNTER_RIGHT_REVERSE 0

/** Forward and reverse counter values. */
static uint8_t counter_left_frw, counter_left_rev,
	       counter_right_frw, counter_right_rev;
/** Last TCNT values. */
static uint8_t counter_left_old, counter_right_old;
/** Overall counter values. */
uint16_t counter_left, counter_right;
/** Counter differences since last update.
 * Maximum of 9 significant bits, sign included. */
int16_t counter_left_diff, counter_right_diff;

/** Initialize the counters. */
void
counter_init (void)
{
    /* Left counter. */
    /*  External, rising edge. */
    TCCR2 = regv (FOC2, WGM20, COM21, COM20, WGM21, CS22, CS21, CS20,
		     0,     0,     0,     0,     0,    1,    1,    1);
    /* Right counter. */
    /*  Normal counter. */
    TCCR3A = 0;
    /*  External, rising edge. */
    TCCR3B = regv (ICNC3, ICES3, 5, WGM33, WGM32, CS32, CS31, CS30,
		       0,     0, 0,	0,     0,    1,    1,    1);
    /* Begin with safe values. */
    counter_left_frw = 0;
    counter_left_rev = 0;
    counter_left_old = TCNT2;
    counter_right_frw = 0;
    counter_right_rev = 0;
    counter_right_old = TCNT3L;
    /* Interrupts for direction. */
    EICRB = 0x05;
    EIFR = _BV (4) | _BV (5);
    EIMSK = _BV (4) | _BV (5);
}

/** Left direction change. */
SIGNAL (SIG_INTERRUPT4)
{
    uint8_t c;
    c = TCNT2;
    if (PINE & _BV (4))
      {
	counter_left_rev += c - counter_left_old;
	LED1 (!COUNTER_LEFT_REVERSE);
      }
    else
      {
	counter_left_frw += c - counter_left_old;
	LED1 (COUNTER_LEFT_REVERSE);
      }
    counter_left_old = c;
}

/** Right direction change. */
SIGNAL (SIG_INTERRUPT5)
{
    uint8_t c;
    c = TCNT3L;
    if (PINE & _BV (5))
      {
	counter_right_rev += c - counter_right_old;
	LED2 (!COUNTER_RIGHT_REVERSE);
      }
    else
      {
	counter_right_frw += c - counter_right_old;
	LED2 (COUNTER_RIGHT_REVERSE);
      }
    counter_right_old = c;
}

/** Update overall counter values and compute diffs. */
void
counter_update (void)
{
    uint8_t c;
    /* Disable ints. */
    EIMSK &= ~(_BV (4) | _BV (5));
    /* Read left counter. */
    c = TCNT2;
    if (PINE & _BV (4))
	counter_left_frw += c - counter_left_old;
    else
	counter_left_rev += c - counter_left_old;
    counter_left_old = c;
    /* Read right counter. */
    c = TCNT3L;
    if (PINE & _BV (5))
	counter_right_frw += c - counter_right_old;
    else
	counter_right_rev += c - counter_right_old;
    counter_right_old = c;
    /* Update counter values and diffs. */
#if COUNTER_LEFT_REVERSE == 0
    counter_left_diff = counter_left_frw;
    counter_left_diff -= counter_left_rev;
#else
    counter_left_diff = counter_left_rev;
    counter_left_diff -= counter_left_frw;
#endif
    counter_left_frw = 0;
    counter_left_rev = 0;
    counter_left += counter_left_diff;
#if COUNTER_RIGHT_REVERSE == 0
    counter_right_diff = counter_right_frw;
    counter_right_diff -= counter_right_rev;
#else
    counter_right_diff = counter_right_rev;
    counter_right_diff -= counter_right_frw;
#endif
    counter_right_frw = 0;
    counter_right_rev = 0;
    counter_right += counter_right_diff;
    /* Enable ints. */
    EIMSK |= _BV (4) | _BV (5);
}

