/* codewheel.h */
/* Codewheel sensors management. {{{
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

#ifndef _CODEWHEEL_H
#define _CODEWHEEL_H

#define CODEWHEEL_CPR 499
#define CODEWHEEL_TIMER_TASK_PERIOD 10L
typedef enum
{
	CODEWHEEL_INIT,
	CODEWHEEL_REQUEST_REBASE,
	CODEWHEEL_REBASED
} TCodewheel_state;

typedef struct
{
	TCodewheel_state state;
	uint16_t rebase_offset;
	uint16_t time;
} codewheel_s;

/* This function initializes the codewheel optical sensors and associated interrupt */
void codewheel_init(void);

/* This function resets the wheel position */
void codewheel_reset(void);

/* This function returns the codewheel state */
TCodewheel_state codewheel_get_state(void);

/* This function modify the codewheel state */
void codewheel_set_state(TCodewheel_state state);

/* This function returns the wheel position */
uint16_t codewheel_get_value(void);

/* This function returns the offset value */
uint16_t codewheel_get_rebase_offset(void);

/* This function saves the counter value used when a codewheel reset is requested */
void codewheel_set_rebase_offset(uint16_t offset);

/* This function converts the angle value from row format to degrees */
float codewheel_convert_angle_raw2degrees(uint16_t raw_value);

/* This function converts the angle value from row format to radians */
float codewheel_convert_angle_raw2radians(uint16_t raw_value);

/* Task for turn time measurment */
void codewheel_timer_task(void);

/* This function start the codewheel timer task */
void start_codewheel_timer_task(void);

/* This function stop the codewheel timer task */
void stop_codewheel_timer_task(void);

#endif
