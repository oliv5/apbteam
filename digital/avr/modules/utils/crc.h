#ifndef crc_h
#define crc_h
/* crc.h */
/* crc - Compute CRC for data (CRC on 8 bits). {{{
 *
 * Copyright (C) 2009 Dufour Jérémy
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

#ifndef HOST
#include <util/crc16.h>
#endif

#ifdef HOST
/**
 * CRC i-button, 8-bit (Dallas/Maxim) for host.
 */
uint8_t
_crc_ibutton_update (uint8_t crc, uint8_t data)
{   
    uint8_t i;

    crc = crc ^ data;
    for (i = 0; i < 8; i++)
      {   
	if (crc & 0x01)
	    crc = (crc >> 1) ^ 0x8C;
	else
	    crc >>= 1;
      }
    return crc;
}
#endif

/**
 * Compute CRC.
 * @param data the data on which to compute CRC.
 * @param length the length of data (in byte).
 * @return the computed CRC.
 */
static inline uint8_t
crc_compute (uint8_t *data, uint8_t length)
{
    uint8_t i, crc;
    /* Sanity check. */
    if (!data)
	return 0;
    /* For each data byte. */
    for (i = 0, crc = 0; i < length; i++)
      {
	/* Compute CRC. */
	crc = _crc_ibutton_update (crc, data[i]);
      }
    /* Return computed CRC. */
    return crc;
}

#endif /* crc_h */
