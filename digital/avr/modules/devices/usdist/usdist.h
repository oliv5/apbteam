#ifndef usdist_h
#define usdist_h
/* usdist.h */
/* usdist - Analog US distance sensor support. {{{
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

/*
 * Manage ultrasonic distance sensors using analog to digital converter.
 *
 * AC_USDIST_SENSORS should be set to a list of space separated USDIST_SENSOR
 * macro calls:
 *
 * USDIST_SENSOR (adc_number, synchro_io_port, synchro_io_n)
 */

/** Minimal calibrated distance. */
#define USDIST_MM_MIN 100

/** Maximal calibrated distance. */
#define USDIST_MM_MAX 700

/** Distance considered as too far to be true. */
#define USDIST_MM_TOO_FAR 650

/** Array containing the last measures in millimeters. */
extern uint16_t usdist_mm[AC_USDIST_NB];

/** Initialise module. */
void
usdist_init (void);

/** To be called every cycle to update sensor measures.
 * - returns: non zero if sensor value has been updated. */
uint8_t
usdist_update (void);

#endif /* usdist_h */
