/* counter_ext.avr.c - External counter support. */
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
#include "counter.h"
#include "io.h"

/**
 * This file add support for an external counter like the hdlcounter or
 * avrcounter project.  This can be better in order not to loose steps and
 * support more counters.
 */

/** Define the left counter address. */
#define COUNTER_LEFT 0
/** Define the right counter address. */
#define COUNTER_RIGHT 1
/** Define the first auxiliary counter address. */
#define COUNTER_AUX0 2

/** Define to 1 to reverse the left counter. */
#define COUNTER_LEFT_REVERSE 0
/** Define to 1 to reverse the right counter. */
#define COUNTER_RIGHT_REVERSE 0
/** Define to 1 to reverse the first auxiliary counter. */
#define COUNTER_AUX0_REVERSE 0

/** Define to 1 to use the AVR External Memory system, or 0 to use hand made
 * signals. */
#define COUNTER_USE_XMEM 1

/** Last values. */
static uint8_t counter_left_old, counter_right_old, counter_aux0_old;
/** Overall counter values. */
uint16_t counter_left, counter_right, counter_aux0;
/** Counter differences since last update.
 * Maximum of 7 significant bits, sign included. */
int16_t counter_left_diff, counter_right_diff, counter_aux0_diff;

#if !COUNTER_USE_XMEM
# define COUNTER_ALE _BV (2)
# define COUNTER_RD _BV (1)
# define COUNTER_WR _BV (0)
#endif

/** Read an external counter. */
static inline uint8_t
counter_read (uint8_t n)
{
#if COUNTER_USE_XMEM
    uint8_t * const ext = (void *) 0x1100;
    return ext[n];
#else
    uint8_t v;
    PORTA = n;
    PORTG &= ~COUNTER_ALE;
    PORTA = 0;
    DDRA = 0;
    PORTG &= ~COUNTER_RD;
    utils_nop ();
    utils_nop ();
    v = PINA;
    PORTG |= COUNTER_RD;
    PORTG |= COUNTER_ALE;
    DDRA = 0xff;
    return v;
#endif
}

/** Initialize the counters. */
void
counter_init (void)
{
#if COUNTER_USE_XMEM
    /* Long wait-states. */
    XMCRA = _BV (SRW11);
    /* Do not use port C for address. */
    XMCRB = _BV (XMM2) | _BV (XMM1) | _BV (XMM0);
    /* Long wait-states and enable. */
    MCUCR |= _BV (SRE) | _BV (SRW10);
#else
    PORTG |= COUNTER_ALE | COUNTER_RD | COUNTER_WR;
    DDRG |= COUNTER_ALE | COUNTER_RD | COUNTER_WR;
    PORTA = 0;
    DDRA = 0xff;
#endif
    /* Begin with safe values. */
    counter_left_old = counter_read (COUNTER_LEFT);
    counter_right_old = counter_read (COUNTER_RIGHT);
    counter_aux0_old = counter_read (COUNTER_AUX0);
}

/** Update overall counter values and compute diffs. */
void
counter_update (void)
{
    uint8_t left, right, aux0;
    /* Sample counters. */
    left = counter_read (COUNTER_LEFT);
    right = counter_read (COUNTER_RIGHT);
    aux0 = counter_read (COUNTER_AUX0);
    /* Left counter. */
#if !COUNTER_LEFT_REVERSE
    counter_left_diff = (int8_t) (left - counter_left_old);
#else
    counter_left_diff = (int8_t) (counter_left_old - left);
#endif
    counter_left += counter_left_diff;
    /* Right counter. */
#if !COUNTER_RIGHT_REVERSE
    counter_right_diff = (int8_t) (right - counter_right_old);
#else
    counter_right_diff = (int8_t) (counter_right_old - right);
#endif
    counter_right += counter_right_diff;
    /* First auxiliary counter. */
#if !COUNTER_AUX0_REVERSE
    counter_aux0_diff = (int8_t) (aux0 - counter_aux0_old);
#else
    counter_aux0_diff = (int8_t) (counter_aux0_old - aux0);
#endif
    counter_aux0 += counter_aux0_diff;
}

