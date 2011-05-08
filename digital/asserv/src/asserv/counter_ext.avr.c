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

#include "modules/utils/utils.h"
#include "modules/math/fixed/fixed.h"
#include "io.h"

/**
 * This file add support for an external counter like the hdlcounter or
 * avrcounter project.  This can be better in order not to loose steps and
 * support more counters.
 *
 * There is additionnal support for error correction on the right counter.
 */

/** Define the left counter address. */
#define COUNTER_LEFT 0
/** Define the right counter address. */
#define COUNTER_RIGHT 1
/** Define the first auxiliary counter address. */
#define COUNTER_AUX0 2
/** Define the second auxiliary counter address. */
#define COUNTER_AUX1 3

/** Define to 1 to reverse the left counter. */
#define COUNTER_LEFT_REVERSE 1
/** Define to 1 to reverse the right counter. */
#define COUNTER_RIGHT_REVERSE 0
/** Define to 1 to reverse the first auxiliary counter. */
#define COUNTER_AUX0_REVERSE 1
/** Define to 1 to reverse the second auxiliary counter. */
#define COUNTER_AUX1_REVERSE 1

/** Left counter shift. */
#define COUNTER_LEFT_SHIFT 0
/** Right counter shift. */
#define COUNTER_RIGHT_SHIFT 0
/** First auxiliary counter shift. */
#define COUNTER_AUX0_SHIFT 0
/** Second auxiliary counter shift. */
#define COUNTER_AUX1_SHIFT 0

/** Define to 1 to use the AVR External Memory system, or 0 to use hand made
 * signals. */
#define COUNTER_USE_XMEM 1

/** Last values. */
static uint16_t counter_left_old, counter_right_old,
		counter_aux_old[AC_ASSERV_AUX_NB];
/** New values, being updated by step update. */
static int16_t counter_left_new_step, counter_right_new_step,
	       counter_aux_new_step[AC_ASSERV_AUX_NB];
/** Last raw step values */
static uint8_t counter_left_old_step, counter_right_old_step,
	       counter_aux_old_step[AC_ASSERV_AUX_NB];
/** Overall counter values. */
uint16_t counter_left, counter_right,
	 counter_aux[AC_ASSERV_AUX_NB];
/** Overall uncorrected counter values. */
static int32_t counter_right_raw;
/** Correction factor (f8.24). */
uint32_t counter_right_correction = 1L << 24;
/** Counter differences since last update.
 * Maximum of 9 significant bits, sign included. */
int16_t counter_left_diff, counter_right_diff,
	counter_aux_diff[AC_ASSERV_AUX_NB];

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
    counter_left_old_step = counter_read (COUNTER_LEFT);
    counter_right_old_step = counter_read (COUNTER_RIGHT);
    counter_aux_old_step[0] = counter_read (COUNTER_AUX0);
    counter_aux_old_step[1] = counter_read (COUNTER_AUX1);
}

/** Update one step.  If counters are not read fast enough, they could
 * overflow, call this function often to update step counters. */
void
counter_update_step (void)
{
    uint8_t left, right, aux0, aux1;
    int8_t diff;
    /* Sample counters. */
    left = counter_read (COUNTER_LEFT);
    right = counter_read (COUNTER_RIGHT);
    aux0 = counter_read (COUNTER_AUX0);
    aux1 = counter_read (COUNTER_AUX1);
    /* Update step counters. */
    diff = (int8_t) (left - counter_left_old_step);
    counter_left_old_step = left;
    counter_left_new_step += diff;
    diff = (int8_t) (right - counter_right_old_step);
    counter_right_old_step = right;
    counter_right_new_step += diff;
    diff = (int8_t) (aux0 - counter_aux_old_step[0]);
    counter_aux_old_step[0] = aux0;
    counter_aux_new_step[0] += diff;
    diff = (int8_t) (aux1 - counter_aux_old_step[1]);
    counter_aux_old_step[1] = aux1;
    counter_aux_new_step[1] += diff;
}

/** Update overall counter values and compute diffs. */
void
counter_update (void)
{
    /* Wants fresh data. */
    counter_update_step ();
    /* Left counter. */
    uint16_t left = counter_left_new_step;
    left &= 0xffff << COUNTER_LEFT_SHIFT; /* Reset unused bits. */
#if !COUNTER_LEFT_REVERSE
    counter_left_diff = (int16_t) (left - counter_left_old);
#else
    counter_left_diff = (int16_t) (counter_left_old - left);
#endif
    counter_left_diff >>= COUNTER_LEFT_SHIFT;
    counter_left_old = left;
    counter_left += counter_left_diff;
    /* Right counter. */
    uint16_t right = counter_right_new_step;
    right &= 0xffff << COUNTER_RIGHT_SHIFT; /* Reset unused bits. */
#if !COUNTER_RIGHT_REVERSE
    counter_right_diff = (int16_t) (right - counter_right_old);
#else
    counter_right_diff = (int16_t) (counter_right_old - right);
#endif
    counter_right_diff >>= COUNTER_RIGHT_SHIFT;
    counter_right_old = right;
    /* Fix right counter. */
    counter_right_raw += counter_right_diff;
    uint16_t right_new = fixed_mul_f824 (counter_right_raw,
					 counter_right_correction);
    counter_right_diff = (int16_t) (right_new - counter_right);
    counter_right = right_new;
    /* First auxiliary counter. */
    uint16_t aux0 = counter_aux_new_step[0];
    aux0 &= 0xffff << COUNTER_AUX0_SHIFT; /* Reset unused bits. */
#if !COUNTER_AUX0_REVERSE
    counter_aux_diff[0] = (int16_t) (aux0 - counter_aux_old[0]);
#else
    counter_aux_diff[0] = (int16_t) (counter_aux_old[0] - aux0);
#endif
    counter_aux_diff[0] >>= COUNTER_AUX0_SHIFT;
    counter_aux_old[0] = aux0;
    counter_aux[0] += counter_aux_diff[0];
    /* Second auxiliary counter. */
    uint16_t aux1 = counter_aux_new_step[1];
    aux1 &= 0xffff << COUNTER_AUX1_SHIFT; /* Reset unused bits. */
#if !COUNTER_AUX1_REVERSE
    counter_aux_diff[1] = (int16_t) (aux1 - counter_aux_old[1]);
#else
    counter_aux_diff[1] = (int16_t) (counter_aux_old[1] - aux1);
#endif
    counter_aux_diff[1] >>= COUNTER_AUX1_SHIFT;
    counter_aux_old[1] = aux1;
    counter_aux[1] += counter_aux_diff[1];
}

