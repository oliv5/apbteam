/* elevator.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
 *
 * Copyright (C) 2009 Nicolas Haller
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
#include "elevator.h"

/**
 * State of the elevator
 */
uint8_t elvt_is_ready = 0;

/**
 * nb puck in the elevator
 */
uint8_t elvt_nb_puck = 0;

/**
 * new_puck information (from filterbridge)
 */
uint8_t elvt_new_puck = 0;

/**
 * elevator orders
 */
elvt_order_e elvt_order_position = CLOSE;
uint8_t elvt_order_in_progress = 0;
uint8_t elvt_degraded_mode = 0;
uint8_t elvt_position_required = 0;

/**
 * We prepare the elevator
*/
void
elvt_prepare(uint8_t pos)
{
    if(elvt_order_position == CLOSE)
      {
	elvt_order_in_progress = 1;
	elvt_order_position = PREPARE;
	elvt_position_required = pos;
      }
    else /* if we are already prepared, just say it's done */
	elvt_order_in_progress = 0;
}

/**
 * We open the elevator
*/
void
elvt_open(uint8_t pos)
{
    if(elvt_order_position == PREPARE)
      {
	elvt_order_in_progress = 1;
	elvt_order_position = OPEN;
	elvt_position_required = pos;
	elvt_degraded_mode = 0;
      }
    else /* We are already open, just say it's done and whistle */
	elvt_order_in_progress = 0;
}

/**
 * We open the elevator in degradad mode
*/
void
elvt_open_degraded(uint8_t pos)
{
    if(elvt_order_position == PREPARE)
      {
	elvt_order_in_progress = 1;
	elvt_order_position = OPEN;
	elvt_position_required = pos;
	elvt_degraded_mode = 1;
      }
    else /* We are already open, just say it's done and whistle */
	elvt_order_in_progress = 0;
}


/**
 * We close the elevator and go away
*/
void
elvt_close(void)
{
    if(elvt_order_position == OPEN)
      {
	elvt_order_in_progress = 1;
	elvt_order_position = CLOSE;
      }
    else /* We are already close, just say it's done and play maracas */
	elvt_order_in_progress = 0;
}

/* +AutoDec */
/* -AutoDec */

