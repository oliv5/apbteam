/* recovery.h */
/* Beacon recovery mode. {{{
 *
 * Copyright (C) 2012 Florent Duchon
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

#ifndef _RECOVERY_H
#define _RECOVERY_H

#include "position.h"

#define MAX_POINT_RECOVERY  					500
#define RECOVERY_TRICKY_THRESHOLD  			MAX_POINT_RECOVERY/3

/* Status returns */
typedef enum 
{
	RECOVERY_IN_PROGRESS,
	RECOVERY_FINISHED,
	RECOVERY_IGNORE_ANGLE_NEXT_TIME
} TRecoveryStatus;

/* This function is used to calculate all obstacle positions from sractch */
TRecoveryStatus recovery(coord_s * new_point,opponent_s result[MAX_OBSTACLE]);

#endif
