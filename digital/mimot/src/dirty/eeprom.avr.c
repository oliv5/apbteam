/* eeprom.avr.c - Save parameters to internal EEPROM. */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
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
#include "eeprom.h"

#include "modules/utils/byte.h"

#include <avr/eeprom.h>

#include "cs.h"

#define EEPROM_START 0

/* WARNING:
 * If you change EEPROM format, be sure to change the EEPROM_KEY in header if
 * your new format is not compatible with the old one or you will load
 * garbages in parameters. */

/* Read parameters from eeprom. */
void
eeprom_read_params (void)
{
    uint8_t *p8 = (uint8_t *) EEPROM_START;
    uint16_t *p16;
    if (eeprom_read_byte (p8++) != EEPROM_KEY)
	return;
    cs_aux[0].speed.max = eeprom_read_byte (p8++);
    cs_aux[1].speed.max = eeprom_read_byte (p8++);
    cs_aux[0].speed.slow = eeprom_read_byte (p8++);
    cs_aux[1].speed.slow = eeprom_read_byte (p8++);
    output_set_reverse (&output_aux[0], eeprom_read_byte (p8++));
    output_set_reverse (&output_aux[1], eeprom_read_byte (p8++));
    p16 = (uint16_t *) p8;
    cs_aux[0].speed.acc = eeprom_read_word (p16++);
    cs_aux[1].speed.acc = eeprom_read_word (p16++);
    cs_aux[0].pos.kp = eeprom_read_word (p16++);
    cs_aux[1].pos.kp = eeprom_read_word (p16++);
    cs_aux[0].pos.ki = eeprom_read_word (p16++);
    cs_aux[1].pos.ki = eeprom_read_word (p16++);
    cs_aux[0].pos.kd = eeprom_read_word (p16++);
    cs_aux[1].pos.kd = eeprom_read_word (p16++);
    cs_aux[0].blocking_detection.error_limit = eeprom_read_word (p16++);
    cs_aux[0].blocking_detection.speed_limit = eeprom_read_word (p16++);
    cs_aux[0].blocking_detection.counter_limit = eeprom_read_word (p16++);
    cs_aux[1].blocking_detection.error_limit = eeprom_read_word (p16++);
    cs_aux[1].blocking_detection.speed_limit = eeprom_read_word (p16++);
    cs_aux[1].blocking_detection.counter_limit = eeprom_read_word (p16++);
    cs_aux[0].pos.e_sat = eeprom_read_word (p16++);
    cs_aux[0].pos.i_sat = eeprom_read_word (p16++);
    cs_aux[0].pos.d_sat = eeprom_read_word (p16++);
    cs_aux[1].pos.e_sat = eeprom_read_word (p16++);
    cs_aux[1].pos.i_sat = eeprom_read_word (p16++);
    cs_aux[1].pos.d_sat = eeprom_read_word (p16++);
}

/* Write parameters to eeprom. */
void
eeprom_write_params (void)
{
    uint8_t *p8 = (uint8_t *) EEPROM_START;
    uint16_t *p16;
    eeprom_write_byte (p8++, EEPROM_KEY);
    eeprom_write_byte (p8++, cs_aux[0].speed.max);
    eeprom_write_byte (p8++, cs_aux[1].speed.max);
    eeprom_write_byte (p8++, cs_aux[0].speed.slow);
    eeprom_write_byte (p8++, cs_aux[1].speed.slow);
    eeprom_write_byte (p8++, output_aux[0].reverse);
    eeprom_write_byte (p8++, output_aux[1].reverse);
    p16 = (uint16_t *) p8;
    eeprom_write_word (p16++, cs_aux[0].speed.acc);
    eeprom_write_word (p16++, cs_aux[1].speed.acc);
    eeprom_write_word (p16++, cs_aux[0].pos.kp);
    eeprom_write_word (p16++, cs_aux[1].pos.kp);
    eeprom_write_word (p16++, cs_aux[0].pos.ki);
    eeprom_write_word (p16++, cs_aux[1].pos.ki);
    eeprom_write_word (p16++, cs_aux[0].pos.kd);
    eeprom_write_word (p16++, cs_aux[1].pos.kd);
    eeprom_write_word (p16++, cs_aux[0].blocking_detection.error_limit);
    eeprom_write_word (p16++, cs_aux[0].blocking_detection.speed_limit);
    eeprom_write_word (p16++, cs_aux[0].blocking_detection.counter_limit);
    eeprom_write_word (p16++, cs_aux[1].blocking_detection.error_limit);
    eeprom_write_word (p16++, cs_aux[1].blocking_detection.speed_limit);
    eeprom_write_word (p16++, cs_aux[1].blocking_detection.counter_limit);
    eeprom_write_word (p16++, cs_aux[0].pos.e_sat);
    eeprom_write_word (p16++, cs_aux[0].pos.i_sat);
    eeprom_write_word (p16++, cs_aux[0].pos.d_sat);
    eeprom_write_word (p16++, cs_aux[1].pos.e_sat);
    eeprom_write_word (p16++, cs_aux[1].pos.i_sat);
    eeprom_write_word (p16++, cs_aux[1].pos.d_sat);
}

/* Clear eeprom parameters. */
void
eeprom_clear_params (void)
{
    uint8_t *p = (uint8_t *) EEPROM_START;
    eeprom_write_byte (p, 0xff);
}

