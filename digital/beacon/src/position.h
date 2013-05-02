/* position.h */
/* Beacon triangulation algorithms. {{{
 *
 * Copyright (C) 2011 Florent Duchon
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

#ifndef _POSITION_H
#define _POSITION_H

#include <stdint.h>

#define MAX_OBSTACLE 		1
#define MAX_BEACON			3
#define MAX_TEMP_POSITION	MAX_OBSTACLE * 2
#define OBSTACLE_RADIUS		100
#define IGNORE_ANGLE			1000

/* Status returns */
typedef enum{
	POSITION_NO_ERROR,
	POSITION_IGNORE_ANGLE
} TPositionStatus;

typedef enum{
	OPPONENT_1 = 1,
	OPPONENT_2
} TOpponent_ID;

typedef enum{
	POV_BEACON_1 = 1,
	POV_BEACON_2,
	POV_BEACON_3
} TBeacon_ID;


typedef enum{
	X,
	Y
} TCoord_type;

/* Structures definition */

/* Beacon Structure */
typedef struct
{
	uint16_t angleNumber;					
	float angle[MAX_OBSTACLE+1];
}beacon_s;

/* Robot structure */
typedef struct
{
	int16_t x;
	int16_t y;
	int8_t trust;
	float angle[MAX_BEACON+1];
}robot_s;


/* Coordinates structure */
typedef struct
{
	int x;
	int y;
}coord_s;

/* Recovery Structure */
typedef struct
{
	int x;
	int y;
	int occurence;
}recovery_s;

/* This function is used to initialize all needed structures */
void position_init_struct(void);

/* This function update the opponent position when a new angle is avalaible */
int update_position(uint16_t beaconID, uint16_t angleID, float angle);

/* This function returns the requested coord according to the opponent number */
int16_t position_get_coord(TOpponent_ID id, TCoord_type type);

/* This function returns the trust according to opponent number */
int8_t position_get_trust(TOpponent_ID id);

/* This function returns the latest angle to a specified beacon */
float position_get_beacon_angle(TBeacon_ID id);

#endif