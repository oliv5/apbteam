/* test_isp.c */
/* avr.isp - Serial programming AVR module. {{{
 *
 * Copyright (C) 2009 Nicolas Schodet
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

#include "modules/isp/isp.h"

int
main (void)
{
    /* Just testing compilation. */
    return 0;
}

void
AC_ISP_SPI_ENABLE (void)
{
}

void
AC_ISP_SPI_DISABLE (void)
{
}

void
AC_ISP_SPI_SCK_PULSE (void)
{
}

uint8_t
AC_ISP_SPI_TX (uint8_t data)
{
    return 0;
}

void
AC_ISP_FRAME_SEND_CHAR (uint8_t data)
{
}

