#ifndef ia_h
#define ia_h
/* ia.h */
/*  {{{
 *
 * Copyright (C) 2008 Nélio Laranjeiro
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
#include "asserv.h"

#define TABLE_MAX_Y 3000
#define TABLE_MAX_X 2100

#define DISTRIBUTOR_SAMPLES_Y 2100

#define DISTRIBUTOR_SAMPLES_BLUE_X 700
#define DISTRIBUTOR_SAMPLES_RED_X 2300

#define DISTRIBUTOR_ICE_Y 1350

#define ASSERV_ARM_ROTATION_INIT 0
#define ASSERV_ARM_ROTATION_FULL 5000
#define ASSERV_ARM_ROTATION_THIRD (ASSERV_ARM_ROTATION_FULL / 3)

#define ASSERV_ARM_SPEED_FULL 100
#define ASSERV_ARM_SPEED_HALF (ASSERV_ARM_SPEED_FULL / 2)

enum ia_team_e
{
    TEAM_COLOR_RED,
    TEAM_COLOR_BLUE
};

struct ia_t
{
    /* Bool status of the previous sequence loaded. 
     * If the previous was the sequence ISISI the next one shall be SISIS (S =
     * sample, I = ice).
     * false = ISISI.
     * true = SISIS.
     */
    bool sequence;

    /* Bool status indicating our ice distributor status. */
    bool ice_status_our;

    /* Team color */
    bool team_color;
};


/** Initialize the IA.
  */
void
ia_init (void);

#endif /* ia_h */
