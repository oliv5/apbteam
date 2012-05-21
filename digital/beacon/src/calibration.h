/* calibration.h */
/* Beacon servomotor calibration. {{{
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

#ifndef _CALIBRATION_H
#define _CALIBRATION_H

#include "servo.h"

#define CALIBRATION_FAST_TASK_PERIOD 		22L
#define CALIBRATION_SLOW_TASK_PERIOD 	22L

#define FAST_SCANNING_OFFSET 		20
#define SCANNING_STEP 			1

typedef enum
{
	SET_SERVO_1,
	SET_SERVO_2,
	CLEAR
} TLaser_flag_type;

typedef enum
{
	FAST_SCANNING,
	SLOW_SCANNING
} TScanning_mode;

typedef enum
{
	CALIBRATION_INIT,
	CALIBRATION_FAST_SCANNING,
	CALIBRATION_SLOW_SCANNING,
	SCANNING_STATE_CALIBRATED
} TCalibration_state;

typedef struct
{
	TCalibration_state state;
	TLaser_flag_type laser_flag;
} calibration_s;

/* This function iniatializes the calibration structure*/
void calibration_init_structure(void);

/* This function starts the calibration task */
void calibration_start_task(void);

/* This function stops the calibration task */
void calibration_stop_task(void);

/* This function starts or stops the calibration task depending on the current state */
void calibration_start_stop_task(void);

/* This function restarts the calibration task changing its frequency */
void calibration_change_task_frequency(uint32_t frequency);

/* Calibration task is used to calibrated the servo motors in order to run the laser parallel to the table */
void calibration_task(void);

/* This function drives the defined servo motor in order to scan verticaly */
void calibration_scanning(TServo_ID servo_id);

/* This function sets the laser flag according the given value SET or CLEAR */
void calibration_set_laser_flag(TLaser_flag_type value);

/* This function returns the laser flag */
TLaser_flag_type calibration_get_laser_flag(void);

/* This function returns the calibration state */
TCalibration_state calibration_get_state(void);

#endif
