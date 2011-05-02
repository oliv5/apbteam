#ifndef twi_master_h
#define twi_master_h
/* twi_master.h */
/* ai - Robot Artificial Intelligence. {{{
 *
 * Copyright (C) 2010 Nicolas Schodet
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
 * Handle communication as a master to several slaves.
 *
 * The communication protocol is always based on:
 *  - a message sent to the slave with a command to execute,
 *  - a status read from the slave containing the current slave state.
 *
 * The first byte of all messages is a CRC of the following bytes, called the
 * message payload.
 *
 * The first byte of payload sent to slave is the command sequence number.  It
 * is used by the master to know if its command has been handled.  The last
 * handled command sequence number is available in the slave status (third
 * byte).
 *
 * As long as the slave last command sequence number is not equal to the last
 * sent sequence number, the master can not send any other command.  If the
 * slave do not acknowledge the command after a time out, the command is sent
 * again.
 *
 * Several commands can be stored by this module, it will defer their
 * transmission until the first command is acknowledged.
 */

/** Initialise module. */
void
twi_master_init (void);

/** Synchronise all slaves, return non zero if synchronised. */
uint8_t
twi_master_sync (void);

/** Get a buffer to send a command to the corresponding slave. */
uint8_t *
twi_master_get_buffer (uint8_t slave);

/** Send previously got buffer with the given length. */
void
twi_master_send_buffer (uint8_t length);

#endif /* twi_master_h */
