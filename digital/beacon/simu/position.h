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

#define LARGEUR_TABLE 2000
#define LONGUEUR_TABLE 3000
#define LARGEUR_DEMI_TABLE LARGEUR_TABLE/2
#define M_PI 3.14

typedef struct
{
	float angle[4];
	int last_updated_beacon;
	int before_last_updated_beacon;
	
}beacon_value_s;

typedef struct
{
	int x;
	int y;
	int old_x;
	int old_y;
}opponent_s;
 
typedef struct
{
	int lost_beacon;
}status_s;

void init_struct(void);
void update_position(int beacon_number,double angle);
void distance_filter(void);

#endif