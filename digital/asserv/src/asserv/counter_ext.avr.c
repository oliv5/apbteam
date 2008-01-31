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

/**
 * This file add support for an external counter like the hdlcounter or
 * avrcounter project.  This can be better in order not to loose steps.
 */

/** Last values. */
static uint8_t counter_left_old, counter_right_old;
/** Overall counter values. */
static uint16_t counter_left, counter_right;
/** Counter differences since last update.
 * Maximum of 7 significant bits, sign included. */
static int16_t counter_left_diff, counter_right_diff;

#define COUNTER_ALE _BV (2)
#define COUNTER_RD _BV (1)
#define COUNTER_WR _BV (0)

static inline void
counter_restart (void);

/** Initialize the counters. */
static inline void
counter_init (void)
{
    PORTG |= COUNTER_ALE | COUNTER_RD | COUNTER_WR;
    DDRG |= COUNTER_ALE | COUNTER_RD | COUNTER_WR;
    PORTA = 0;
    DDRA = 0xff;
    /* Begin with safe values. */
    counter_restart ();
}

/** Read an external counter. */
static inline uint8_t
counter_read (uint8_t n)
{
    uint8_t v;
    PORTA = n;
    PORTG &= ~COUNTER_ALE;
    PORTA = 0;
    DDRA = 0;
    PORTG &= ~COUNTER_RD;
    v = PINA;
    PORTG |= COUNTER_RD;
    PORTG |= COUNTER_ALE;
    DDRA = 0xff;
    return v;
}

/** Update overall counter values and compute diffs. */
static inline void
counter_update (void)
{
    uint8_t left, right;
    left = counter_read (0);
    counter_left_diff = (int8_t) (left - counter_left_old);
    counter_left += counter_left_diff;
    right = counter_read (1);
    counter_right_diff = (int8_t) (right - counter_right_old);
    counter_right += counter_right_diff;
}

/** Restart counting. */
static inline void
counter_restart (void)
{
    counter_left_old = counter_read (0);
    counter_right_old = counter_read (1);
}

