#ifndef timer_h
#define timer_h
/* timer.h */
/* ai - Robot Artificial Intelligence. {{{
 *
 * Copyright (C) 2009 Dufour Jérémy
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
 * Tick timer, used to slow down the main loop to a given rate.
 */

/**
 * Prescaler configured for timer/counter 0.
 * If you want to change this value, you also need to change the TCCR0
 * register.
 */
#define TIMER_TC0_PRESCALER 256L

/**
 * Top configured for timer/counter 0.
 * If you want to change this value, you also need to change the TCCR0
 * register.
 */
#define TIMER_TC0_TOP 255

/** Period of timer, ms. */
#define TIMER_PERIOD_MS \
    (1000.0 / (AC_FREQ / (TIMER_TC0_PRESCALER * (TIMER_TC0_TOP + 1))))

/** Initialise timer. */
void
timer_init (void);

/**
 * Wait until next tick.  Return non zero if we are late.
 *
 * Warning: if this function return non zero, it means we are late and the
 * main loop is lasting more than the time configured. Consequence: some
 * important functions will not work correctly!
 */
uint8_t
timer_wait (void);

/** Return current timer value, for performance measurement. */
uint8_t
timer_get (void);

/** Get a tick value, incremented at each tick, never reset. */
uint16_t
timer_get_tick (void);

#endif /* timer_h */
