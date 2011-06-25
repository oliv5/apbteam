#ifndef pawn_sensor_h
#define pawn_sensor_h
/* pawn_sensor.h */
/* robospierre - Eurobot 2011 AI. {{{
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

/** Update sensor state and return the element type if an element is ready to
 * be taken. */
uint8_t
pawn_sensor_get (uint8_t direction);

/** Update pawn sensors. */
void
pawn_sensor_update (void);

/** Enable bumpers. */
void
pawn_sensor_bumper_enable (uint8_t enabled);

/** Temporarily disable bumpers. */
void
pawn_sensor_bumper_wait (uint16_t wait);

/** Return last bumped pawn. */
vect_t
pawn_sensor_get_last_bumped (void);

#endif /* pawn_sensor_h */
