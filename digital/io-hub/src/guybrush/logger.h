#ifndef logger_h
#define logger_h
/* logger.h */
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

/** Log a serie of bytes. */
#define logger_log(args...) \
    do { \
	logger_write_event_counter (); \
	PREPROC_FOR (logger_log_, ## args) \
    } while (0)
#define logger_log_(b) logger_write (b);

#ifndef HOST

/** Initialise logger, do not write anything yet. */
void
logger_init (void);

/** Write pending information. */
void
logger_update (void);

/** Write event counter for synchronisation. */
void
logger_write_event_counter (void);

/** Write one byte in buffer. */
void
logger_write (uint8_t b);

#else

#define logger_init()
#define logger_update()
#define logger_write_event_counter()
#define logger_write(b)

#endif

#endif /* logger_h */
