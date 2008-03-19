#ifndef eeprom_h
#define eeprom_h
/* eeprom.h */
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

/**
 * @file Store and load configuration parameters from the EEPROM of the AVR.
 * This module is able to store some parameters for the io program and its
 * sub-modules, to load them.
 * To prevent problem when you change the format of your the parameters (or
 * add/remove some), it uses a key to store the version of the parameters
 * organization. So when you change something, update the key.
 */

/**
 * Load parameters from the EEPROM.
 * Check if the key is correct.
 */
void
eeprom_load_param ();

/**
 * Store parameters in the EEPROM.
 */
void
eeprom_save_param ();

/**
 * Clear parameters in the EEPROM by invalidating the key.
 */
void
eeprom_clear_param ();

#endif /* eeprom_h */
