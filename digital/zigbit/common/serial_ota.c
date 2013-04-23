/* serial_ota.c */
/* Serial over ZB interface. {{{
 *
 * Copyright (C) 2012 Florent Duchon
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

static uint8_t serial_over_zigbit_flag = 0;
static uint16_t serial_over_zibgit_addr = 0;

/* This function stops the uart over zb mode */
void serial_over_zigbit_stop(void)
{
	serial_over_zigbit_flag = 0;
}

/* This function starts the uart over zb mode */
void serial_over_zigbit_start(uint16_t addr)
{
	serial_over_zibgit_addr = addr;
	serial_over_zigbit_flag = 1;
}

/* This function returns the serial ota mode */
uint8_t get_serial_ota_mode(void)
{
	return serial_over_zigbit_flag;
}

/* This function returns the serial ota ED address */
uint16_t get_serial_ota_EDaddr(void)
{
	return serial_over_zibgit_addr;
}