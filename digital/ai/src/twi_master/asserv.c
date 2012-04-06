/* asserv.c */
/* ai - Robot Artificial Intelligence. {{{
 *
 * Copyright (C) 2008 Dufour Jérémy
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
#include "asserv.h"

#include "twi_master.h"

#include "modules/utils/byte.h"
#include "modules/math/fixed/fixed.h"
#include "bot.h"
#include "io.h"

/**
 * Flag bit position value for the status byte of the asserv.
 */
enum asserv_status_flag_e
{
    /** Bot movement finished with success. */
    asserv_status_flag_move_succeed = 0,
    /** Bot movement finished with failure: the bot is blocked. */
    asserv_status_flag_move_failed = 1,
    /** Bot is moving forward (linear speed greater than 0). */
    asserv_status_flag_move_forward = 2,
    /** Bot is moving backward (linear speed smaller than 0). */
    asserv_status_flag_move_backward = 3,
#if AC_ASSERV_AUX_NB
    /** Motor0 movement finished with success. */
    asserv_status_flag_motor0_succeed = 4,
    /** Motor0 movement finished with failure. */
    asserv_status_flag_motor0_failed = 5,
    /** Motor1 movement finished with success. */
    asserv_status_flag_motor1_succeed = 6,
    /** Motor1 movement finished with failure. */
    asserv_status_flag_motor1_failed = 7,
#endif
};
typedef enum asserv_status_flag_e asserv_status_flag_e;

/** Scaling factor. */
static uint32_t asserv_scale;

/** Scaling factor inverse. */
static uint32_t asserv_scale_inv;

/** Last moving direction. */
static uint8_t asserv_last_moving_direction;

/**
 * Structure for storing a position for the bot using asserv units.
 */
typedef struct asserv_position_t
{
    /** X position. */
    uint32_t x;
    /** Y position. */
    uint32_t y;
    /** Angle. */
    uint16_t a;
} asserv_position_t;

/**
 * Status structure maintains by the update command.
 */
typedef struct asserv_struct_s
{
    /** Status flags. */
    uint8_t status;
    /** Asserv board input port. */
    uint8_t input_port;
    /** Bot position. */
    asserv_position_t position;
#if AC_ASSERV_AUX_NB
    /** Motor0 position. */
    uint16_t motor0_position;
    /** Motor1 position. */
    uint16_t motor1_position;
#endif
} asserv_struct_s;

/**
 * Status variable.
 */
asserv_struct_s asserv_status;

/** Set scale.
 * @param scale number of millimeter per step (f8.24).
 */
static void
asserv_set_scale (uint32_t scale)
{
    asserv_scale = scale;
    asserv_scale_inv = fixed_div_f824 (1L << 24, scale);
}

void
asserv_init (void)
{
    asserv_set_scale (BOT_SCALE * (1L << 24));
}

void
asserv_status_cb (uint8_t *status)
{
    /* Parse received data and store them. */
    asserv_status.status = status[0];
    asserv_status.input_port = status[1];
    asserv_status.position.x = v8_to_v32 (0, status[3], status[4], status[5]);
    asserv_status.position.y = v8_to_v32 (0, status[6], status[7], status[8]);
    asserv_status.position.a = v8_to_v16 (status[9], status[10]);
#if AC_ASSERV_AUX_NB
    asserv_status.motor0_position = v8_to_v16 (status[11], status[12]);
    asserv_status.motor1_position = v8_to_v16 (status[13], status[14]);
#endif
    /* Update moving direction. */
    if (asserv_get_moving_direction () != 0)
	asserv_last_moving_direction = asserv_get_moving_direction ();
}

asserv_status_e
asserv_move_cmd_status (void)
{
    /* Check Motor Finished flag */
    if (asserv_status.status & _BV (asserv_status_flag_move_succeed))
	return success;
    /* Check Motor Blocked flag */
    else if (asserv_status.status & _BV (asserv_status_flag_move_failed))
	return failure;
    /* Otherwise, not finished nor failure */
    return none;
}

#if AC_ASSERV_AUX_NB

asserv_status_e
asserv_motor0_cmd_status (void)
{
    /* Check Motor0 Finished flag */
    if (asserv_status.status & _BV (asserv_status_flag_motor0_succeed))
	return success;
    /* Check Motor0 Blocked flag */
    else if (asserv_status.status & _BV (asserv_status_flag_motor0_failed))
	return failure;
    /* Otherwise, not finished nor failure */
    return none;
}

asserv_status_e
asserv_motor1_cmd_status (void)
{
    /* Check Motor1 Finished flag */
    if (asserv_status.status & _BV (asserv_status_flag_motor1_succeed))
	return success;
    /* Check Motor1 Blocked flag */
    else if (asserv_status.status & _BV (asserv_status_flag_motor1_failed))
	return failure;
    /* Otherwise, not finished nor failure */
    return none;
}

#endif /* AC_ASSERV_AUX_NB */

void
asserv_get_position (position_t *current_position)
{
    assert (current_position);
    /* Copy last received status buffer information to current position */
    current_position->v.x = fixed_mul_f824 (asserv_status.position.x,
					    asserv_scale);
    current_position->v.y = fixed_mul_f824 (asserv_status.position.y,
					    asserv_scale);
    current_position->a = asserv_status.position.a;
}

#if AC_ASSERV_AUX_NB

uint16_t
asserv_get_motor0_position (void)
{
    /* Return the position of the motor0 of the current status buffer */
    return asserv_status.motor0_position;
}

uint16_t
asserv_get_motor1_position (void)
{
    /* Return the position of the motor1 of the current status buffer */
    return asserv_status.motor1_position;
}

#endif /* AC_ASSERV_AUX_NB */

uint8_t
asserv_get_moving_direction (void)
{
    /* Foward move? */
    if (asserv_status.status & _BV (asserv_status_flag_move_forward))
	return DIRECTION_FORWARD;
    /* Backward move? */
    if (asserv_status.status & _BV (asserv_status_flag_move_backward))
	return DIRECTION_BACKWARD;
    /* Not moving */
    return DIRECTION_NONE;
}

uint8_t
asserv_get_last_moving_direction (void)
{
    return asserv_last_moving_direction;
}

void
asserv_reset (void)
{
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'z';
    twi_master_send_buffer (1);
}

void
asserv_free_motor (void)
{
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'w';
    twi_master_send_buffer (1);
}

void
asserv_stop_motor (void)
{
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 's';
    twi_master_send_buffer (1);
}

void
asserv_move_linearly (int32_t distance)
{
    distance = fixed_mul_f824 (distance, asserv_scale_inv);
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'l';
    buffer[1] = v32_to_v8 (distance, 2);
    buffer[2] = v32_to_v8 (distance, 1);
    buffer[3] = v32_to_v8 (distance, 0);
    twi_master_send_buffer (4);
}

void
asserv_move_angularly (int16_t angle)
{
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'a';
    buffer[1] = v16_to_v8 (angle, 1);
    buffer[2] = v16_to_v8 (angle, 0);
    twi_master_send_buffer (3);
}

void
asserv_goto_angle (int16_t angle)
{
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'y';
    buffer[1] = v16_to_v8 (angle, 1);
    buffer[2] = v16_to_v8 (angle, 0);
    twi_master_send_buffer (3);
}

void
asserv_goto_xya (uint32_t x, uint32_t y, int16_t a, uint8_t backward)
{
    x = fixed_mul_f824 (x, asserv_scale_inv);
    y = fixed_mul_f824 (y, asserv_scale_inv);
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'X';
    buffer[1] = v32_to_v8 (x, 2);
    buffer[2] = v32_to_v8 (x, 1);
    buffer[3] = v32_to_v8 (x, 0);
    buffer[4] = v32_to_v8 (y, 2);
    buffer[5] = v32_to_v8 (y, 1);
    buffer[6] = v32_to_v8 (y, 0);
    buffer[7] = v16_to_v8 (a, 1);
    buffer[8] = v16_to_v8 (a, 0);
    buffer[9] = backward;
    twi_master_send_buffer (10);
}

void
asserv_go_to_the_wall (uint8_t backward)
{
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'f';
    buffer[1] = backward;
    twi_master_send_buffer (2);
}

void
asserv_push_the_wall (uint8_t backward, uint32_t init_x, uint32_t init_y,
		      uint16_t init_a)
{
    if (init_x != (uint32_t) -1)
	init_x = fixed_mul_f824 (init_x, asserv_scale_inv);
    if (init_y != (uint32_t) -1)
	init_y = fixed_mul_f824 (init_y, asserv_scale_inv);
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'G';
    buffer[1] = backward;
    buffer[2] = v32_to_v8 (init_x, 2);
    buffer[3] = v32_to_v8 (init_x, 1);
    buffer[4] = v32_to_v8 (init_x, 0);
    buffer[5] = v32_to_v8 (init_y, 2);
    buffer[6] = v32_to_v8 (init_y, 1);
    buffer[7] = v32_to_v8 (init_y, 0);
    buffer[8] = v16_to_v8 (init_a, 1);
    buffer[9] = v16_to_v8 (init_a, 0);
    twi_master_send_buffer (10);
}

#if AC_ASSERV_AUX_NB

void
asserv_move_motor0_absolute (uint16_t position, uint8_t speed)
{
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'b';
    buffer[1] = v16_to_v8 (position, 1);
    buffer[2] = v16_to_v8 (position, 0);
    buffer[3] = speed;
    twi_master_send_buffer (4);
}

void
asserv_move_motor1_absolute (uint16_t position, uint8_t speed)
{
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'c';
    buffer[1] = v16_to_v8 (position, 1);
    buffer[2] = v16_to_v8 (position, 0);
    buffer[3] = speed;
    twi_master_send_buffer (4);
}

#endif /* AC_ASSERV_AUX_NB */

void
asserv_set_x_position (int32_t x)
{
    x = fixed_mul_f824 (x, asserv_scale_inv);
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'p';
    buffer[1] = 'X';
    buffer[2] = v32_to_v8 (x, 2);
    buffer[3] = v32_to_v8 (x, 1);
    buffer[4] = v32_to_v8 (x, 0);
    twi_master_send_buffer (5);
}

void
asserv_set_y_position (int32_t y)
{
    y = fixed_mul_f824 (y, asserv_scale_inv);
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'p';
    buffer[1] = 'Y';
    buffer[2] = v32_to_v8 (y, 2);
    buffer[3] = v32_to_v8 (y, 1);
    buffer[4] = v32_to_v8 (y, 0);
    twi_master_send_buffer (5);
}

void
asserv_set_angle_position (int16_t angle)
{
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'p';
    buffer[1] = 'A';
    buffer[2] = v32_to_v8 (angle, 1);
    buffer[3] = v32_to_v8 (angle, 0);
    twi_master_send_buffer (4);
}

void
asserv_set_speed (uint8_t linear_high, uint8_t angular_high,
		  uint8_t linear_low, uint8_t angular_low)
{
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'p';
    buffer[1] = 's';
    buffer[2] = linear_high;
    buffer[3] = angular_high;
    buffer[4] = linear_low;
    buffer[5] = angular_low;
    twi_master_send_buffer (6);
}

void
asserv_set_position (int32_t x, int32_t y, int16_t angle)
{
    x = fixed_mul_f824 (x, asserv_scale_inv);
    y = fixed_mul_f824 (y, asserv_scale_inv);
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'p';
    buffer[1] = 'X';
    buffer[2] = v32_to_v8 (x, 2);
    buffer[3] = v32_to_v8 (x, 1);
    buffer[4] = v32_to_v8 (x, 0);
    buffer[5] = 'Y';
    buffer[6] = v32_to_v8 (y, 2);
    buffer[7] = v32_to_v8 (y, 1);
    buffer[8] = v32_to_v8 (y, 0);
    buffer[9] = 'A';
    buffer[10] = v32_to_v8 (angle, 1);
    buffer[11] = v32_to_v8 (angle, 0);
    twi_master_send_buffer (12);
}

void
asserv_goto (uint32_t x, uint32_t y, uint8_t backward)
{
    x = fixed_mul_f824 (x, asserv_scale_inv);
    y = fixed_mul_f824 (y, asserv_scale_inv);
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'x';
    buffer[1] = v32_to_v8 (x, 2);
    buffer[2] = v32_to_v8 (x, 1);
    buffer[3] = v32_to_v8 (x, 0);
    buffer[4] = v32_to_v8 (y, 2);
    buffer[5] = v32_to_v8 (y, 1);
    buffer[6] = v32_to_v8 (y, 0);
    buffer[7] = backward;
    twi_master_send_buffer (8);
}

#if AC_ASSERV_AUX_NB

void
asserv_motor0_zero_position (int8_t speed)
{
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'B';
    buffer[1] = speed;
    twi_master_send_buffer (2);
}

void
asserv_motor1_zero_position (int8_t speed)
{
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'C';
    buffer[1] = speed;
    twi_master_send_buffer (2);
}

void
asserv_motor0_free (void)
{
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'r';
    buffer[1] = 0;
    twi_master_send_buffer (2);
}

void
asserv_motor1_free (void)
{
    uint8_t *buffer = twi_master_get_buffer (ASSERV_SLAVE);
    buffer[0] = 'r';
    buffer[1] = 1;
    twi_master_send_buffer (2);
}

#endif /* AC_ASSERV_AUX_NB */
