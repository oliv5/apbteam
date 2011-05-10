#ifndef asserv_h
#define asserv_h
/* asserv.h */
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
#include "defs.h"

/**
 * @file Control the asserv board using the TWI protocol.
 * This files contains the 'public' functions to send commands to the asserv
 * board using a protocol over TWI communication.
 */

/** Slave number in twi_master list. */
#define ASSERV_SLAVE 0

/** Asserv TWI address. */
#define ASSERV_TWI_ADDRESS 4

/** Length of the status buffer (not including CRC). */
#define ASSERV_STATUS_LENGTH 15

/** Use backward movements. */
#define ASSERV_BACKWARD 1
/** Authorise reverse the requested movement direction, may be or'ed with the
 * previous define. */
#define ASSERV_REVERT_OK 2

/** Initialize the asserv control module. */
void
asserv_init (void);

/** Called when a new status buffer is received, update the asserv
 * information. */
void
asserv_status_cb (uint8_t *status);

/**
 * Status of a move or motor class command.
 * It is return by status functions.
 */
typedef enum asserv_status_e
{
    /** No status is available. The command is not finished yet. */
    none,
    /** The command has succeed. */
    success,
    /** The command has failed. The bot or the motor is blocked */
    failure
} asserv_status_e;

/**
 * Is last move class command has successfully ended?
 * This function is used to know the status of the last move command. It looks
 * at the status register.
 * @return the status of the last move class command.
 */
asserv_status_e
asserv_move_cmd_status (void);

/**
 * Is last motor0 class command has successfully ended?
 * This function is used to know the status of the last motor0 command. It
 * looks at the status register.
 * @return the status of the last move class command.
 */
asserv_status_e
asserv_motor0_cmd_status (void);

/**
 * Is last motor1 class command has successfully ended?
 * This function is used to know the status of the last motor1 command. It
 * looks at the status register.
 * @return the status of the last move class command.
 */
asserv_status_e
asserv_motor1_cmd_status (void);

/**
 * Get the current position of the bot.
 * @param current_position the current position to update.
 */
void
asserv_get_position (position_t *current_position);

/**
 * Get the motor0 position.
 * @return the position of the motor0 (in steps).
 */
uint16_t
asserv_get_motor0_position (void);

/**
 * Get the motor1 position.
 * @return the position of the motor1 (in steps).
 */
uint16_t
asserv_get_motor1_position (void);

/**
 * Are we moving forward/backward?
 * @return
 *   - DIRECTION_NONE we are not moving;
 *   - DIRECTION_FORWARD we are moving forward;
 *   - DIRECTION_BACKWARD we are moving backward.
 */
uint8_t
asserv_get_moving_direction (void);

/**
 * Get the last moving direction of the bot.
 * @return DIRECTION_FORWARD or DIRECTION_BACKWARD.
 */
uint8_t
asserv_get_last_moving_direction (void);

/**
 * Reset the asserv board.
 * Other class command.
 */
void
asserv_reset (void);

/**
 * Free the motors (stop controlling them).
 * Other class command.
 */
void
asserv_free_motor (void);

/**
 * Stop the motor (and the bot).
 * Other class command.
 */
void
asserv_stop_motor (void);

/**
 * Move linearly.
 * Move class command.
 * @param distance the distance to move (mm).
 */
void
asserv_move_linearly (int32_t distance);

/**
 * Move angularly (turn).
 * Move class command.
 * @param angle the angle to turn.
 */
void
asserv_move_angularly (int16_t angle);

/**
 * Make the bot turn of an absolute angle.
 * The angle is absolute and not a difference with the current one.
 * @param a the absolute angle
 */
void
asserv_goto_angle (int16_t angle);

/**
 * Go to an absolute position and then an absolute angle.
 * @param x the absolute position on the X axis.
 * @param y the absolute position on the Y axis.
 * @param a the absolute angle.
 * @param backward 0 no backward, ASSERV_BACKWARD backward compulsary,
 * ASSERV_REVERT_OK backward allowed.
 */
void
asserv_goto_xya (uint32_t x, uint32_t y, int16_t a, uint8_t backward);

/**
 * Go to the wall (moving backward).
 * Move class command.
 */
void
asserv_go_to_the_wall (uint8_t backward);

/**
 * Move the motor0.
 * Motor0 class command.
 * This function take the number of steps you want to move to. This is an
 * absolute position.
 * @param position desired goal position (in step).
 * @param speed speed of the movement.
 */
void
asserv_move_motor0_absolute (uint16_t position, uint8_t speed);

/**
 * Move the motor1.
 * Motor1 class command.
 * This function take the number of steps you want to move to. This is an
 * absolute position.
 * @param position desired goal position (in step).
 * @param speed speed of the movement.
 */
void
asserv_move_motor1_absolute (uint16_t position, uint8_t speed);

/**
 * Set current X position.
 * Other class command.
 * @param x X position.
 */
void
asserv_set_x_position (int32_t x);

/**
 * Set current Y position.
 * Other class command.
 * @param y Y position.
 */
void
asserv_set_y_position (int32_t y);

/**
 * Set current angular position.
 * Other class command.
 * @param angle angular position.
 */
void
asserv_set_angle_position (int16_t angle);

/**
 * Set speeds of movements.
 * Other class command.
 * @param linear_high linear high speed
 * @param angular_high angular high speed
 * @param linear_low linear low speed
 * @param angular_low angular low speed
 */
void
asserv_set_speed (uint8_t linear_high, uint8_t angular_high,
		  uint8_t linear_low, uint8_t angular_low);

/**
 * Set the complete position of the bot.
 * This is an helpful function preventing you from calling multiples other
 * ones.
 * It calls other class commands.
 * @param x X position
 * @param y Y position
 * @param angle angular position
 */
void
asserv_set_position (int32_t x, int32_t y, int16_t angle);

/**
 * Go to an absolute position in (X, Y).
 * @param x the x position on the table.
 * @param y the y position on the table.
 * @param backward 0 no backward, ASSERV_BACKWARD backward compulsary,
 * ASSERV_REVERT_OK backward allowed.
 */
void
asserv_goto (uint32_t x, uint32_t y, uint8_t backward);

/** Reset the motor0 to the zero position. */
void
asserv_motor0_zero_position (int8_t speed);

/** Reset the motor1 to the zero position. */
void
asserv_motor1_zero_position (int8_t speed);

/** Set PWM to zero for motor0. */
void
asserv_motor0_free (void);

/** Set PWM to zero for motor1. */
void
asserv_motor1_free (void);

#endif /* asserv_h */
