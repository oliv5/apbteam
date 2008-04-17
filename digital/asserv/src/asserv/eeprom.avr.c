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

#include "counter.h"
#include "pwm.h"
#include "pos.h"
#include "speed.h"
#include "postrack.h"
#include "traj.h"

#define EEPROM_START 0

/* WARNING:
 * If you change EEPROM format, be sure to change the EEPROM_KEY in header if
 * your new format is not compatible with the old one or you will load
 * garbages in parameters. */

static uint32_t
eeprom_read_dword (uint8_t *addr)
{
    uint8_t dw[4];
    dw[0] = eeprom_read_byte (addr++);
    dw[1] = eeprom_read_byte (addr++);
    dw[2] = eeprom_read_byte (addr++);
    dw[3] = eeprom_read_byte (addr++);
    return v8_to_v32 (dw[3], dw[2], dw[1], dw[0]);
}

static void
eeprom_write_dword (uint8_t *addr, uint32_t dw)
{
    eeprom_write_byte (addr++, v32_to_v8 (dw, 0));
    eeprom_write_byte (addr++, v32_to_v8 (dw, 1));
    eeprom_write_byte (addr++, v32_to_v8 (dw, 2));
    eeprom_write_byte (addr++, v32_to_v8 (dw, 3));
}

/* Read parameters from eeprom. */
void
eeprom_read_params (void)
{
    uint8_t *p8 = (uint8_t *) EEPROM_START;
    uint16_t *p16;
    if (eeprom_read_byte (p8++) != EEPROM_KEY)
	return;
    speed_theta.max = eeprom_read_byte (p8++);
    speed_alpha.max = eeprom_read_byte (p8++);
    speed_aux0.max = eeprom_read_byte (p8++);
    speed_theta.slow = eeprom_read_byte (p8++);
    speed_alpha.slow = eeprom_read_byte (p8++);
    speed_aux0.slow = eeprom_read_byte (p8++);
    pwm_set_reverse (eeprom_read_byte (p8++));
    counter_right_correction = eeprom_read_dword (p8); p8 += 4;
    p16 = (uint16_t *) p8;
    postrack_set_footing (eeprom_read_word (p16++));
    speed_theta.acc = eeprom_read_word (p16++);
    speed_alpha.acc = eeprom_read_word (p16++);
    speed_aux0.acc = eeprom_read_word (p16++);
    pos_theta.kp = eeprom_read_word (p16++);
    pos_alpha.kp = eeprom_read_word (p16++);
    pos_aux0.kp = eeprom_read_word (p16++);
    pos_theta.ki = eeprom_read_word (p16++);
    pos_alpha.ki = eeprom_read_word (p16++);
    pos_aux0.ki = eeprom_read_word (p16++);
    pos_theta.kd = eeprom_read_word (p16++);
    pos_alpha.kd = eeprom_read_word (p16++);
    pos_aux0.kd = eeprom_read_word (p16++);
    pos_e_sat = eeprom_read_word (p16++);
    pos_i_sat = eeprom_read_word (p16++);
    pos_d_sat = eeprom_read_word (p16++);
    pos_blocked = eeprom_read_word (p16++);
    traj_eps = eeprom_read_word (p16++);
    traj_aeps = eeprom_read_word (p16++);
}

/* Write parameters to eeprom. */
void
eeprom_write_params (void)
{
    uint8_t *p8 = (uint8_t *) EEPROM_START;
    uint16_t *p16;
    eeprom_write_byte (p8++, EEPROM_KEY);
    eeprom_write_byte (p8++, speed_theta.max);
    eeprom_write_byte (p8++, speed_alpha.max);
    eeprom_write_byte (p8++, speed_aux0.max);
    eeprom_write_byte (p8++, speed_theta.slow);
    eeprom_write_byte (p8++, speed_alpha.slow);
    eeprom_write_byte (p8++, speed_aux0.slow);
    eeprom_write_byte (p8++, pwm_reverse);
    eeprom_write_dword (p8, counter_right_correction); p8 += 4;
    p16 = (uint16_t *) p8;
    eeprom_write_word (p16++, postrack_footing);
    eeprom_write_word (p16++, speed_theta.acc);
    eeprom_write_word (p16++, speed_alpha.acc);
    eeprom_write_word (p16++, speed_aux0.acc);
    eeprom_write_word (p16++, pos_theta.kp);
    eeprom_write_word (p16++, pos_alpha.kp);
    eeprom_write_word (p16++, pos_aux0.kp);
    eeprom_write_word (p16++, pos_theta.ki);
    eeprom_write_word (p16++, pos_alpha.ki);
    eeprom_write_word (p16++, pos_aux0.ki);
    eeprom_write_word (p16++, pos_theta.kd);
    eeprom_write_word (p16++, pos_alpha.kd);
    eeprom_write_word (p16++, pos_aux0.kd);
    eeprom_write_word (p16++, pos_e_sat);
    eeprom_write_word (p16++, pos_i_sat);
    eeprom_write_word (p16++, pos_d_sat);
    eeprom_write_word (p16++, pos_blocked);
    eeprom_write_word (p16++, traj_eps);
    eeprom_write_word (p16++, traj_aeps);
}

/* Clear eeprom parameters. */
void
eeprom_clear_params (void)
{
    uint8_t *p = (uint8_t *) EEPROM_START;
    eeprom_write_byte (p, 0xff);
}

