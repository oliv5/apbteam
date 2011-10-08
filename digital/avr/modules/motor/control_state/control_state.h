#ifndef control_state_h
#define control_state_h
/* control_state.h */
/* motor - Motor control module. {{{
 *
 * Copyright (C) 2011 Nicolas Schodet
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

/** Control modes. */
enum control_state_mode_t
{
    /** No control, direct output. */
    CS_MODE_NONE = 0,
    /** Blocking detection, should only be disabled when a higher layer take
     * care of it.  Disabling it will only disable its effect, blocking
     * detection is still done, but not applied. */
    CS_MODE_BLOCKING_DETECTION = 1,
    /** Position control for linear movement on a polar system. */
    CS_MODE_POS_CONTROL_THETA = 2,
    /** Position control for angular movement on a polar system. */
    CS_MODE_POS_CONTROL_ALPHA = 4,
    /** Position control for single motor system, can also be used with
     * control_state_set_mode to enable both controls on a polar system.
     * If position control is disabled, output should be assigned a value or
     * else it will keep its last computed value. */
    CS_MODE_POS_CONTROL = 4,
    /** Speed control, for speed consign or speed limited position consign. */
    CS_MODE_SPEED_CONTROL = 8,
    /** Trajectory control, higher level control. */
    CS_MODE_TRAJ_CONTROL = 16,
    /** Finished flag, set when a high level order is completed. */
    CS_MODE_FINISHED = 64,
    /** Blocked, output tied to zero. */
    CS_MODE_BLOCKED = 128,
};
typedef enum control_state_mode_t control_state_mode_t;

/** Control state. */
struct control_state_t
{
    /** Mask of currently selected control modes. */
    uint8_t modes;
};
typedef struct control_state_t control_state_t;

/** Set current modes.
 *  - enable: enable all mode lower or equal to this mode.
 *  - disable_mask: if non zero, mask of modes to disable.
 */
static inline void
control_state_set_mode (control_state_t *control_state, uint8_t enable,
			uint8_t disable_mask)
{
    uint8_t enable_mask = enable ? (enable | (enable - 1)) : 0;
    control_state->modes = enable_mask & ~disable_mask;
}

/** Set finished flag, restore blocking detection and full position control if
 * any position control is enabled. */
static inline void
control_state_finished (control_state_t *control_state)
{
    control_state->modes |= CS_MODE_FINISHED;
    if (control_state->modes & (CS_MODE_POS_CONTROL_THETA
				| CS_MODE_POS_CONTROL_ALPHA))
	control_state->modes |= (CS_MODE_POS_CONTROL_THETA
				 | CS_MODE_POS_CONTROL_ALPHA
				 | CS_MODE_BLOCKING_DETECTION);
}

/** Set blocked state, disable all other modes. */
static inline void
control_state_blocked (control_state_t *control_state)
{
    control_state->modes = CS_MODE_BLOCKED;
}

/** Return non zero if finished. */
static inline uint8_t
control_state_is_finished (control_state_t *control_state)
{
    return control_state->modes & CS_MODE_FINISHED;
}

/** Return non zero if blocked. */
static inline uint8_t
control_state_is_blocked (control_state_t *control_state)
{
    return control_state->modes & CS_MODE_BLOCKED;
}

#endif /* control_state_h */
