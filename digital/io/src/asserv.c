/* asserv.c */
/* io - Input & Output with Artificial Intelligence (ai) support on AVR. {{{
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

#include "asserv.h"

#include "modules/twi/twi.h"	/* twi_* */
#include "modules/utils/byte.h"	/* v*_to_v* */

/**
 * Sequence number.
 * It is used for the acknowledge of the command send to the asserv.
 */
static uint8_t asserv_twi_seq;

/**
 * Flag to know if last command has been proceed by the asserv board.
 */
static uint8_t asserv_twi_seq_ack;

/**
 * Shared buffer used to send commands to the asserv.
 */
static uint8_t asserv_twi_buffer[7];

/**
 * Pointer to access the buffer to put the parameters list.
 */
static uint8_t *asserv_twi_buffer_param = &asserv_twi_buffer[2];

/**
 * Update TWI module until request (send or receive) is finished.
 * This functions is blocking.
 */
static inline void
asserv_twi_update (void);

/**
 * Send a command to the asserv board using the TWI module.
 * If the command you want to send have some parameters, you need to fill
 * asserv_twi_buffer_param buffer.
 * @param command the command to send.
 * @param length the length of the parameters list (in byte).
 * @return
 *  - 0 when there is no error
 *  - 1 if there is already a command running on the asserv (not acknowledged
 *  or not finished yet).
 */
static inline uint8_t
asserv_twi_send_command (uint8_t command, uint8_t length);


/**
 * Settings list of the asserv program.
 */
enum asserv_setting_e
{
    POSITION_X,
    POSITION_Y,
    POSITION_ANGULAR,
    SPEED
};

/* Initialize the asserv control module. */
void
asserv_init (void)
{
    /* Initialize TWI with my (io) address */
    twi_init (AC_IO_TWI_ADDRESS);
    /* We are at first command */
    asserv_twi_seq = asserv_twi_seq_ack = 0;
}

/* Update TWI module until request (send or receive) is finished. */
static inline void
asserv_twi_update (void)
{
    while (!twi_ms_is_finished ())
	;
}

/* Send a command to the asserv board using the TWI module. */
static inline uint8_t
asserv_twi_send_command (uint8_t command, uint8_t length)
{
    /* Check we are not doing a command ? */
    if (asserv_twi_seq != asserv_twi_seq_ack)
	return 1;

    /* Put the command into the buffer */
    asserv_twi_buffer[0] = command;
    /* Put the sequence number */
    asserv_twi_buffer[1] = ++asserv_twi_seq;

    /* Send command to the asserv */
    if (twi_ms_send (AC_ASSERV_TWI_ADDRESS, asserv_twi_buffer, length + 2) != 0)
	return -1;

    /* Update until the command is sent */
    asserv_twi_update ();
    return 0;
}

/* Reset the asserv board. */
void
asserv_reset (void)
{
    /* Send the reset command to the asserv board */
    asserv_twi_send_command ('z', 0);
}

/* Free the motors (stop controlling them). */
void
asserv_free_motor (void)
{
    /* Send the free motor command to the asserv board */
    asserv_twi_send_command ('w', 0);
}

/* Stop the motor (and the bot). */
void
asserv_stop_motor (void)
{
    /* Send the stop motor command to the asserv board */
    asserv_twi_send_command ('s', 0);
}

/* Move linearly. */
void
asserv_move_linearly (int32_t distance)
{
    /* Put distance as parameter */
    asserv_twi_buffer_param[0] = v32_to_v8 (distance, 3);
    asserv_twi_buffer_param[1] = v32_to_v8 (distance, 2);
    asserv_twi_buffer_param[2] = v32_to_v8 (distance, 1);
    /* Send the linear move command to the asserv board */
    asserv_twi_send_command ('l', 3);
}

 /* Move angularly (turn). */
void
asserv_move_angularly (int16_t angle)
{
    /* Put angle as parameter */
    asserv_twi_buffer_param[0] = v16_to_v8 (angle, 1);
    asserv_twi_buffer_param[1] = v16_to_v8 (angle, 0);
    /* Send the angular move command to the asserv board */
    asserv_twi_send_command ('a', 2);
}

/* Go to the wall (moving backward). */
void
asserv_go_to_the_wall (void)
{
    /* Send the go the wall command to the asserv board */
    asserv_twi_send_command ('f', 0);
}

/* Move forward to approach a gutter. */
void
asserv_go_to_gutter (void)
{
    /* Send the go the gutter command to the asserv board */
    asserv_twi_send_command ('F', 0);
}

/* Move the arm. */
void
asserv_move_arm (uint16_t position, uint8_t speed)
{
    /* Put position and speed as parameters */
    asserv_twi_buffer_param[0] = v16_to_v8 (position, 1);
    asserv_twi_buffer_param[1] = v16_to_v8 (position, 0);
    asserv_twi_buffer_param[2] = speed;
    /* Send the move the arm command to the asserv board */
    asserv_twi_send_command ('b', 3);
}

/* Set current X position. */
void
asserv_set_x_position (int32_t x)
{
    /* 'X' subcommand */
    asserv_twi_buffer_param[0] = 'X';
    /* Put x position as parameter */
    asserv_twi_buffer_param[1] = v32_to_v8 (x, 3);
    asserv_twi_buffer_param[2] = v32_to_v8 (x, 2);
    asserv_twi_buffer_param[3] = v32_to_v8 (x, 1);
    /* Send the set X position command to the asserv board */
    asserv_twi_send_command ('p', 4);
}

/* Set current Y position. */
void
asserv_set_y_position (int32_t y)
{
    /* 'Y' subcommand */
    asserv_twi_buffer_param[0] = 'Y';
    /* Put y position as parameter */
    asserv_twi_buffer_param[1] = v32_to_v8 (y, 3);
    asserv_twi_buffer_param[2] = v32_to_v8 (y, 2);
    asserv_twi_buffer_param[3] = v32_to_v8 (y, 1);
    /* Send the set Y position command to the asserv board */
    asserv_twi_send_command ('p', 4);
}

/* Set current angular position. */
void
asserv_set_angle_position (int16_t angle)
{
    /* 'A' subcommand */
    asserv_twi_buffer_param[0] = 'A';
    /* Put angle position as parameter */
    asserv_twi_buffer_param[1] = v32_to_v8 (angle, 1);
    asserv_twi_buffer_param[2] = v32_to_v8 (angle, 0);
    /* Send the set angular position command to the asserv board */
    asserv_twi_send_command ('p', 3);
}

/* Set speeds of movements. */
void
asserv_set_speed (uint8_t linear_high, uint8_t angular_high,
		  uint8_t linear_low, uint8_t angular_low)
{
    /* 's' subcommand */
    asserv_twi_buffer_param[0] = 's';
    /* Put speeds as parameters */
    asserv_twi_buffer_param[1] = linear_high;
    asserv_twi_buffer_param[2] = angular_high;
    asserv_twi_buffer_param[3] = linear_low;
    asserv_twi_buffer_param[4] = angular_low;
    /* Send the set speed of movements command to the asserv board */
    asserv_twi_send_command ('p', 5);
}
