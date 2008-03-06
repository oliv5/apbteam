/* twi.host.c */
/* avr.twi - TWI AVR module. {{{
 *
 * Copyright (C) 2008 Nicolas Schodet
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
#include "twi.h"

/* Empty for the moment. */

/** Initialise twi. */
void
twi_init (uint8_t addr)
{
}

#if AC_TWI_SLAVE_ENABLE

/** Récupère dans buffer les données recues en tant qu'esclave */
uint8_t 
twi_sl_poll (uint8_t *buffer, uint8_t size)
{
    return 0;
}

/** Met à jour le buffer de donnée à envoyer */
void 
twi_sl_update (uint8_t *buffer, uint8_t size)
{
}

#endif /* AC_TWI_SLAVE_ENABLE */

#if AC_TWI_MASTER_ENABLE

/** Is the current transaction finished ? */
int8_t 
twi_ms_is_finished (void)
{
    return 0;
}

/** Send len bytes of data to address */
int8_t
twi_ms_send (uint8_t address, uint8_t *data, uint8_t len)
{
    return -1;
}

/** Read len bytes at addresse en put them in data */
int8_t
twi_ms_read (uint8_t address, uint8_t *data, uint8_t len)
{
    return -1;
}

#endif /* AC_TWI_MASTER_ENABLE */

