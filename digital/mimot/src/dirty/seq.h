#ifndef seq_h
#define seq_h
/* seq.h */
/* asserv - Position & speed motor control on AVR. {{{
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
#include "modules/motor/control_state/control_state.h"

/** There is two mechanism to acknowledge long lived command completion.
 *
 * The first one is dedicated to unreliable and full duplex channels like the
 * serial port.  It is based on sequence numbers.  This file implements this
 * system until it can be integrated in proto module.
 *
 * The second one is simpler, but can only be used with reliable and half
 * duplex channels like the TWI bus.  It just use a flag to remember command
 * completion witch is reseted when a new command arrives.  This one is
 * directly implemented in control system. */

/** Current sequence state. */
struct seq_t
{
    /** Sequence number of the currently processed command, should be between
     * 1 and 127.  When a command is received on the serial port it is ignored
     * if its sequence number is equal to the current sequence number.  In
     * this case a duplicated message is inferred. */
    uint8_t cur;
    /** Sequence number of the most recently finished command.  When a command
     * is finished, the current sequence number is copied to this variable. */
    uint8_t finish;
    /** Sequence number of the most recently acknowledged command.  Until this
     * is not equal to the last finished command sequence number, a message is
     * generated on the serial line. */
    uint8_t ack;
};
typedef struct seq_t seq_t;

/** Auxiliary motor states. */
extern seq_t seq_aux[AC_ASSERV_AUX_NB];

/** Start a new command execution, return non zero if this is a new
 * sequence. */
static inline uint8_t
seq_start (seq_t *seq, uint8_t new)
{
    if (new == seq->cur)
	return 0;
    else
      {
	seq->cur = new;
	return 1;
      }
}

/** Acknowledge a command completion and blocked state. */
static inline void
seq_acknowledge (seq_t *seq, uint8_t ack)
{
    seq->ack = ack;
}

/** Update sequence state according to control state. */
static inline void
seq_update (seq_t *seq, control_state_t *state)
{
    if (control_state_is_blocked (state))
	seq->finish = seq->cur | 0x80;
    else if (control_state_is_finished (state))
	seq->finish = seq->cur;
}

#endif /* seq_h */
