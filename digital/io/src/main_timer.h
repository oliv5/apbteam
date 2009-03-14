#ifndef main_timer_h
#define main_timer_h
/* main_timer.h */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
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
 * @file Main timer.
 * The main timer is responsible for making the main loop executed regularly
 * to a specific time (at least).
 *
 * Sometimes, the main loop can be executed less frequently that we want
 * because it takes too much time! This is bad and should be avoided.
 *
 * The main timer used the timer/counter 0 of the AVR.
 */

/**
 * Prescaler configured for timer/counter 0.
 * If you want to change this value, you also need to change the TCCR0
 * register.
 */
#define MT_TC0_PRESCALER 256L
/**
 * Top configured for timer/counter 0.
 * If you want to change this value, you also need to change the TCCR0
 * register.
 */
#define MT_TC0_TOP 255
/**
 * Period of timer/counter 0 (in millisecond).
 */
#define MT_TC0_PERIOD \
    (1000 / (AC_FREQ / (MT_TC0_PRESCALER * (MT_TC0_TOP + 1))))

/**
 * Initialize the main timer to 4.444 ms.
 * This function setup the timer/counter 0 configuration register.
 */
void
main_timer_init (void);

/**
 * Wait until the main timer overflows.
 * @return
 *  - 0 if we are on time (we have not reached overflow before calling this
 *  function).
 *  - 1 if we have already reached overflow.
 * @warning if this function return 1, it means we are late and the main loop
 * is lasting more than the time configured. Consequence, some important
 * functions (like the chronometer for match duration) will not work
 * correctly!
 */
uint8_t
main_timer_wait (void);

#endif /* main_timer_h */
