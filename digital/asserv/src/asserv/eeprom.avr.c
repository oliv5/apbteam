/* eeprom.avr.c - Save parameters to internal EEPROM. */
/* asserv - Position & speed motor control on AVR. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2006.
 *        Web: http://assos.efrei.fr/robot/
 *      Email: robot AT efrei DOT fr
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

#include <avr/eeprom.h>

#include "pwm.h"
#include "pos.h"
#include "speed.h"
#include "postrack.h"

#define EEPROM_START 0

/* Read parameters from eeprom. */
void
eeprom_read_params (void)
{
    uint8_t *p8 = (uint8_t *) EEPROM_START;
    uint16_t *p16;
    if (eeprom_read_byte (p8++) != EEPROM_KEY)
	return;
    speed_theta_max = eeprom_read_byte (p8++);
    speed_alpha_max = eeprom_read_byte (p8++);
    speed_theta_slow = eeprom_read_byte (p8++);
    speed_alpha_slow = eeprom_read_byte (p8++);
    pwm_dir = eeprom_read_byte (p8++);
    p16 = (uint16_t *) p8;
    postrack_set_footing (eeprom_read_word (p16++));
    speed_theta_acc = eeprom_read_word (p16++);
    speed_alpha_acc = eeprom_read_word (p16++);
    pos_theta_kp = eeprom_read_word (p16++);
    pos_alpha_kp = eeprom_read_word (p16++);
    pos_theta_ki = eeprom_read_word (p16++);
    pos_alpha_ki = eeprom_read_word (p16++);
    pos_theta_kd = eeprom_read_word (p16++);
    pos_alpha_kd = eeprom_read_word (p16++);
    pos_e_sat = eeprom_read_word (p16++);
    pos_int_sat = eeprom_read_word (p16++);
    pos_blocked = eeprom_read_word (p16++);
}

/* Write parameters to eeprom. */
void
eeprom_write_params (void)
{
    uint8_t *p8 = (uint8_t *) EEPROM_START;
    uint16_t *p16;
    eeprom_write_byte (p8++, EEPROM_KEY);
    eeprom_write_byte (p8++, speed_theta_max);
    eeprom_write_byte (p8++, speed_alpha_max);
    eeprom_write_byte (p8++, speed_theta_slow);
    eeprom_write_byte (p8++, speed_alpha_slow);
    eeprom_write_byte (p8++, pwm_dir);
    p16 = (uint16_t *) p8;
    eeprom_write_word (p16++, postrack_footing);
    eeprom_write_word (p16++, speed_theta_acc);
    eeprom_write_word (p16++, speed_alpha_acc);
    eeprom_write_word (p16++, pos_theta_kp);
    eeprom_write_word (p16++, pos_alpha_kp);
    eeprom_write_word (p16++, pos_theta_ki);
    eeprom_write_word (p16++, pos_alpha_ki);
    eeprom_write_word (p16++, pos_theta_kd);
    eeprom_write_word (p16++, pos_alpha_kd);
    eeprom_write_word (p16++, pos_e_sat);
    eeprom_write_word (p16++, pos_int_sat);
    eeprom_write_word (p16++, pos_blocked);
}

/* Clear eeprom parameters. */
void
eeprom_clear_params (void)
{
    uint8_t *p = (uint8_t *) EEPROM_START;
    eeprom_write_byte (p, 0xff);
}

