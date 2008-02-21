/* test_spi.c */
/* avr.spi - SPI AVR module. {{{
 *
 * Copyright (C) 2008 Nélio Laranjeiro
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
#include <stdint.h>
#include <common.h>
#include "io.h"
#include "../spi.h"

/** Call back function to be called on the AVR interruption */
void
spi_interruption_function (void *user_data, uint8_t data)
{
}

int
main (void)
{
    uint8_t test [10];
    uint8_t res;

    res = SPI_IT_ENABLE;
    spi_init (res);
    res = SPI_IT_DISABLE;
    spi_init (res);
    res = SPI_ENABLE;
    spi_init (res);
    res = SPI_DISABLE;
    spi_init (res);
    res = SPI_LSB_FIRST;
    spi_init (res);
    res = SPI_MSB_FIRST;
    spi_init (res);
    res = SPI_MASTER;
    spi_init (res);
    res = SPI_SLAVE;
    spi_init (res);
    res = SPI_CPOL_RISING;
    spi_init (res);
    res = SPI_CPOL_FALLING;
    spi_init (res);
    res = SPI_CPHA_SAMPLE;
    spi_init (res);
    res = SPI_CPHA_SETUP;
    spi_init (res);

    //initialise the spi.
    spi_init (0x14);

    test[0] = 0x2;
    test[1] = 0x3;
    test[2] = 0x4;
    
    spi_send (3);

    res = spi_recv ();
    
    return 0;
}

