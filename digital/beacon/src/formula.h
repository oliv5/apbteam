/* formula.h */
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

#ifndef _FORMULA_H
#define _FORMULA_H

/* Formula calibration */
#define LARGEUR_TABLE 						2070
#define LONGUEUR_TABLE 						3070
#define LARGEUR_DEMI_TABLE 					LARGEUR_TABLE/2

#define BLIND_ZONE_PRECISION					0.18 //10 degrees
// #define BLIND_ZONE_PRECISION					0.18/2 //5 degrees 
#define BLIND_ZONE_BEACON1_ANGLE12_MAX		0 + BLIND_ZONE_PRECISION//10 degrees 
#define BLIND_ZONE_BEACON1_ANGLE13_MIN		1.25856505285 - BLIND_ZONE_PRECISION //72 degrees 
#define BLIND_ZONE_BEACON1_ANGLE13_MAX		1.25856505285 + BLIND_ZONE_PRECISION //72 degrees 

#define BLIND_ZONE_BEACON2_ANGLE12_MAX		0 + BLIND_ZONE_PRECISION //10 degrees 
#define BLIND_ZONE_BEACON2_ANGLE23_MIN		1.25856505285 - BLIND_ZONE_PRECISION//72 degrees 
#define BLIND_ZONE_BEACON2_ANGLE23_MAX		1.25856505285 + BLIND_ZONE_PRECISION //72 degrees 

#define BLIND_ZONE_BEACON3_ANGLE13_MIN		1.24765037811 - BLIND_ZONE_PRECISION //70 degrees 
#define BLIND_ZONE_BEACON3_ANGLE13_MAX		1.24765037811 + BLIND_ZONE_PRECISION//70 degrees 
#define BLIND_ZONE_BEACON3_ANGLE23_MIN		1.24765037811 - BLIND_ZONE_PRECISION//70 degrees 
#define BLIND_ZONE_BEACON3_ANGLE23_MAX		1.24765037811 + BLIND_ZONE_PRECISION//70 degrees 


/* Status return */
typedef enum
{
	FORMULA_VALID_POSITION,
	FORMULA_BLIND_ZONE_DETECTED,
	FORMULA_OUT_OF_TABLE,
	FORMULA_UNKNOWN_FORMULA
} TFormulaStatus;


/* This function computes an absolute position using 2 angles from 2 beacons */
TFormulaStatus formula_compute_position(int formula, int current_beacon_ID, float latest_angle, float current_angle, coord_s * result);

/* This function computes coords using formula 3 mode ie for beacon 1 + 2*/
TFormulaStatus formula3_compute_coord(coord_s * position, float angle_beacon1, float angle_beacon2);

/* This function computes coords using formula 4 mode ie for beacon 1 + 3*/
TFormulaStatus formula4_compute_coord(coord_s * position, float angle_beacon1, float angle_beacon3);

/* This function computes coords using formula 5 mode ie for beacon 2 + 3*/
TFormulaStatus formula5_compute_coord(coord_s * position, float angle_beacon2, float angle_beacon3);

#endif
