/* eeprom.c */
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

#include "eeprom.h"

#include "servo.h"		/* SERVO_NUMBER */
#include "trap.h"		/* trap_high_time_pos */
#include <avr/eeprom.h>		/* eeprom_{read,write}_byte */

/**
 * @defgroup EepromPrivate EEPROM module private variables and functions
 * declarations
 * @{
 */

/**
 * What is the address, in the EEPROM, where are stored the parameters values?
 */
#define EEPROM_PARAM_START 0

/**
 * Actual version of the parameters organization.
 */
#define EEPROM_PARAM_KEY 0x01

/** @} */

/* Load parameters from the EEPROM. */
void
eeprom_load_param ()
{
    uint8_t compt;
    /* The parameters start at the given address */
    uint8_t *ptr8 = (uint8_t *) EEPROM_PARAM_START;
    /* Check if the key/version is correct */
    if (eeprom_read_byte (ptr8++) != EEPROM_PARAM_KEY)
	/* Error, stop here */
	return;

    /* Load trap module data */
    /* Extreme position of the servos motor (high time value of the PWM) */
    for (compt = 0; compt < SERVO_NUMBER; compt++)
      {
	trap_high_time_pos[0][compt] = eeprom_read_byte(ptr8++);
	trap_high_time_pos[1][compt] = eeprom_read_byte(ptr8++);
      }
}

/* Store parameters in the EEPROM. */
void
eeprom_save_param ()
{
    uint8_t compt;
    /* The parameters start at the given address */
    uint8_t *ptr8 = (uint8_t *) EEPROM_PARAM_START;
    /* Store the key */
    eeprom_write_byte (ptr8++, EEPROM_PARAM_KEY);

    /* Store trap module data */
    /* Extreme position of the servos motor (high time value of the PWM) */
    for (compt = 0; compt < SERVO_NUMBER; compt++)
      {
	eeprom_write_byte (ptr8++, trap_high_time_pos[0][compt]);
	eeprom_write_byte (ptr8++, trap_high_time_pos[1][compt]);
      }
}

/* Clear parameters in the EEPROM by invalidating the key. */
void
eeprom_clear_param ()
{
    /* Write an invalid key */
    eeprom_write_byte ((uint8_t *) EEPROM_PARAM_START, 0xFF);
}
