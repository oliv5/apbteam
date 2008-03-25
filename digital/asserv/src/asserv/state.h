#ifndef state_h
#define state_h
/* state.h */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2008 Nicolas Schodet
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

/** There is two mechanism to acknowledge long lived command completion.
 *
 * The first one is dedicated to unreliable and full duplex channels like the
 * serial port.  It is based on sequence numbers.
 *
 * The second one is simpler, but can only be used with reliable and half
 * duplex channels like the TWI bus.  It just use a flag to remember command
 * completion witch is reseted when a new command arrives. */

/** Control mode. */
enum state_mode_t
{
    /** Simple PWM setup mode. */
    MODE_PWM,
    /** Position control mode. */
    MODE_POS,
    /** Speed control mode. */
    MODE_SPEED,
    /** Trajectory control mode. */
    MODE_TRAJ,
};

/** Current motor state. */
struct state_t
{
    /** Current motor control mode. */
    uint8_t mode;
    /** Sequence number of the currently processed command, should be between
     * 1 and 127.  When a command is received on the serial port it is ignored
     * if its sequence number is equal to the current sequence number.  In
     * this case a duplicated message is inferred. */
    uint8_t sequence;
    /** Sequence number of the most recently finished command.  When a command
     * is finished, the current sequence number is copied to this variable. */
    uint8_t sequence_finish;
    /** Sequence number of the most recently acknowledged command.  Until this
     * is not equal to the last finished command sequence number, a message is
     * generated on the serial line. */
    uint8_t sequence_ack;
    /** Simpler flag based mechanism, indicates if the last received command
     * is finished. */
    uint8_t finished;
    /** Wether the motor is blocked. */
    uint8_t blocked;
};

/** Main motors state. */
struct state_t state_main;

/** First auxiliary motor state. */
struct state_t state_aux0;

/** Start a new command execution. */
static inline void
state_start (struct state_t *motor, uint8_t sequence)
{
    motor->sequence = sequence;
    motor->finished = 0;
    motor->blocked = 0;
}

/** Signal the current command completion. */
static inline void
state_finish (struct state_t *motor)
{
    motor->sequence_finish = motor->sequence;
    motor->finished = 1;
}

/** Signal the current command is blocked, disable motor control until a new
 * command is given. */
static inline void
state_blocked (struct state_t *motor)
{
    motor->sequence_finish = motor->sequence | 0x80;
    motor->blocked = 1;
    motor->mode = MODE_PWM;
}

/** Acknowledge a command completion and blocked state. */
static inline void
state_acknowledge (struct state_t *motor, uint8_t sequence)
{
    motor->sequence_ack = sequence;
    if (sequence == motor->sequence_finish)
      {
	motor->finished = 0;
	motor->blocked = 0;
      }
}

#endif /* state_h */
