/* top.c */
/* guybrush - Eurobot 2012 AI. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
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
#include "common.h"
#include "io.h"

#include "playground_2012.h"
#include "asserv.h"

#define FSM_NAME AI
#include "fsm.h"

#include "move.h"
#include "chrono.h"
#include "contact.h"

/*
 * Here is the top FSM.  This FSM is suppose to give life to the robot with an
 * impression of intelligence... Well...
 */

FSM_INIT

FSM_STATES (
	    /* Initial state. */
	    TOP_START)

FSM_START_WITH (TOP_START)

/** Top context. */
struct top_t
{
};

/** Global context. */
struct top_t top_global;
#define ctx top_global

