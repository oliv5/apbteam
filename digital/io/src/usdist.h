#ifndef usdist_h
#define usdist_h
/* usdist.h */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2010 Nicolas Schodet
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

/*
 * Manage ultrasonic distance sensors using analog to digital converter.
 */

/** Number of sensors. */
#define USDIST_NB 4

/** Minimal calibrated distance. */
#define USDIST_MM_MIN 100

/** Maximal calibrated distance. */
#define USDIST_MM_MAX 700

/** Distance considered as too far to be true. */
#define USDIST_MM_TOO_FAR 650

/** Measuring period in cycles. */
#define USDIST_PERIOD_CYCLE (uint8_t) (8.0 / MT_TC0_PERIOD)

/** Array containing the last measures in millimeters. */
extern uint16_t usdist_mm[USDIST_NB];

/** Initialise module. */
void
usdist_init (void);

/** To be called every cycle to update sensor measures.
 * - returns: non zero if sensor value has been updated. */
uint8_t
usdist_update (void);

#endif /* usdist_h */
