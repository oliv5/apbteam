/* customisation.c */
/* Beacon customisation values. {{{
 *
 * Copyright (C) 2013 Florent Duchon
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

#ifndef _CUSTOMISATION_H
#define _CUSTOMISATION_H

/* This function returns the calibration target angle according to the targetID and the beacon UID */
int custom_get_mire_angle(int number,uint16_t uid);

/* This function returns the servo ID according to the UID and calibration servo order */
TServo_ID custom_get_servoID_order(uint8_t order,uint16_t uid);

#endif