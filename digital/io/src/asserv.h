#ifndef asserv_h
#define asserv_h
/* asserv.h */
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

/**
 * @file Control the asserv board from io using the TWI protocol.
 * This files contains the 'public' functions to send commands to the asserv
 * board using a protocol over TWI communication.
 * @see trunk/digital/io/doc/proto_asserv.txt
 * @todo
 *  - the function to send a new command to the asserv board is protected from
 *  sending one when the previous is not finished yet. But the information is
 *  not raised to the upper layer (io). It can be a bad idea to give this
 *  information to the upper layer because it can be mis-interpreted or
 *  ignored.
 */

/**
 * Initialize the asserv control module.
 * This functions does not initialize the asserv board, but the underling
 * communication protocol used to communicate with the asserv (TWI).
 */
void
asserv_init (void);

/**
 * Update the status of the asserv board seen from the io program.
 * This command asks the status buffer of the asserv, receive it and do some
 * internal updates.
 * You need to call this command regularly in order to be able to send new
 * commands (this module will not let you send new command if the previous one
 * has not been yet received).
 */
void
asserv_update_status (void);

/**
 * Is last command sent to the asserv board is being executed?
 * This function is used to know if the last command sent to the asserv board
 * has been received and is currently executing.
 * @return
 *  - 0 if the command has not started to be executed.
 *  - 1 if the command is currently been executed.
 */
uint8_t
asserv_last_cmd_ack (void);

/**
 * Re-send command if not acknowledged.
 * This function should be called when the command has not been acknowledged
 * by the asserv board. It will re send the last command when a certain number
 * of cycle has been reached without any acknowledge from the asserv.
 * @return
 *   - 0 if the command was not received.
 *   - 1 if the command was re-sent.
 */
uint8_t
asserv_retransmit (void);

/**
 * Status of a move or arm class command.
 * It is return by status functions.
 */
typedef enum asserv_status_e
{
    /** No status is available. The command is not finished yet. */
    none,
    /** The command has succeed. */
    success,
    /** The command has failed. The bot or the arm is blocked */
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
 * Is last arm class command has successfully ended?
 * This function is used to know the status of the last arm command. It looks
 * at the status register.
 * @return the status of the last move class command.
 */
asserv_status_e
asserv_arm_cmd_status (void);

/**
 * Structure for storing a position for the bot.
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
 * Get the current position of the bot.
 * @param current_position the current position to update.
 */
void
asserv_get_position (asserv_position_t *current_position);

/**
 * Get the arm position.
 * @return the position of the arm (in steps).
 */
uint16_t
asserv_get_arm_position (void);

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
 * @param distance the distance to move (in step).
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
 * @param a the absolute angle in degrees
 */
void
asserv_goto_angle (int16_t angle);

/**
 * Go to the wall (moving backward).
 * Move class command.
 */
void
asserv_go_to_the_wall (void);

/**
 * Move forward to approach a distributor.
 * Move class command.
 */
void
asserv_go_to_distributor (void);

/**
 * Move the arm to a certain number of steps.
 * Arm class command.
 * This function take the number of steps you want to move to. This is not an
 * absolute position, this is only the number of steps you want to add to the
 * current position.
 * @param offset number of steps to add to the current position.
 * @param speed speed of the movement.
 */
void
asserv_move_arm (int16_t offset, uint8_t speed);

/**
 * Move the arm to close the input hole.
 * It will compute the forward offset to close the input hole by moving the
 * arm in front of it.
 * @todo backward/forward selection support.
 */
void
asserv_close_input_hole (void);

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
 * Go to an absolute position in (X, Y).
 * @param x the x position on the table.
 * @param y the y position on the table.
 */
void
asserv_goto (uint32_t x, uint32_t y);

/**
 * Set the notifier of get samples FSM when the arm reach desired position.
 * You should called this function from the get sample FSM to tell the asserv
 * module you want to be notified when the arm reached the desired position.
 * @param position the desired position of the arm (absolute). Note, 0 is a
 * reset value do disable the notifier. If you want to use it, please add 1.
 */
void
asserv_arm_set_position_reached (uint16_t position);

/**
 * Check if notification of the get sample FSM is required in term of
 * position of the arm.
 * @return
 *   - 0 if the notification is not needed ;
 *   - 1 if the notification of the get sample FSM is required.
 */
uint8_t
asserv_arm_position_reached (void);

#endif /* asserv_h */
