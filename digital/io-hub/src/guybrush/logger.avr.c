/* logger.avr.c */
/* guybrush - Eurobot 2012 AI. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
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
#include "logger.h"

#include <avr/eeprom.h>

/** Record log in EEPROM, bufferize data not to disturb real time
 * constrains.
 *
 * Special bytes:
 *  0xf0 - 0xfd: event counter, to be used for synchronisation.
 *  0xfe: escape character.
 *  0xff: overflow.
 * */

/** Maximum buffer size. */
#define LOGGER_BUF_SIZE 32

/** Maximum log size. */
#define LOGGER_SIZE (E2END + 1)

/** Code writen on overflow. */
#define LOGGER_OVERFLOW 0xff

/** Code writen to escape characters. */
#define LOGGER_ESCAPE 0xfe

/** Special code mask. */
#define LOGGER_SPECIAL 0xf0

/** Event counter rollover value. */
#define LOGGER_EVENT_COUNTER_NB 14

/** Logger context. */
struct logger_t
{
    /** Log buffer. */
    uint8_t buf[LOGGER_BUF_SIZE];
    /** Head and tail in buffer. */
    uint8_t head, tail;
    /** Current log pointer. */
    uint16_t current;
    /** Event counter, incremented at each event, used for synchronisation. */
    uint8_t event_counter;
} logger;

EEMEM uint8_t logger_eeprom[LOGGER_SIZE];

void
logger_init (void)
{
    /* Read last log event counter and choose another value. */
    logger.event_counter = eeprom_read_byte (&logger_eeprom[0]);
    logger.event_counter = (logger.event_counter + 1) %
	LOGGER_EVENT_COUNTER_NB;
}

void
logger_update (void)
{
    if (eeprom_is_ready () && logger.head != logger.tail
	&& logger.current < LOGGER_SIZE)
      {
	/* Write next byte. */
	uint8_t next = logger.buf[logger.head];
	eeprom_write_byte (&logger_eeprom[logger.current++], next);
	logger.head = (logger.head + 1) % LOGGER_BUF_SIZE;
      }
}

static void
logger_write_internal (uint8_t b)
{
    uint8_t tail_new = (logger.tail + 1) % LOGGER_BUF_SIZE;
    if (tail_new == logger.head)
      {
	/* Overflow! */
	logger.buf[logger.tail] = LOGGER_OVERFLOW;
      }
    else
      {
	logger.tail = tail_new;
	logger.buf[tail_new] = b;
      }
}

void
logger_write_event_counter (void)
{
    logger_write_internal (LOGGER_SPECIAL | logger.event_counter);
    logger.event_counter = (logger.event_counter + 1) %
	LOGGER_EVENT_COUNTER_NB;
}

void
logger_write (uint8_t b)
{
    if ((b & LOGGER_SPECIAL) == LOGGER_SPECIAL)
      {
	uint8_t tail_after = (logger.tail + 2) % LOGGER_BUF_SIZE;
	if (tail_after == logger.head)
	    logger_write_internal (LOGGER_OVERFLOW);
	else
	  {
	    logger_write_internal (LOGGER_ESCAPE);
	    logger_write_internal (b);
	  }
      }
    else
	logger_write_internal (b);
}
