/* test_twi_slave.c */
/* avr.twi.slave - TWI slave module. {{{
 *
 * Copyright (C) 2005 Clément Demonchy
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
#include "modules/twi/twi.h"
#include "modules/uart/uart.h"
#include "io.h"

int
main (void)
{
    uint8_t data[TWI_SL_RCPT_SIZE];
    sei ();
    uart0_init ();
    uart0_putc ('s');
    uart0_putc ('s');
    uart0_putc ('s');
    twi_init (0x02);
    data[0] = 0;
    while (42)
      {
	if (twi_sl_poll (data, TWI_SL_RCPT_SIZE))
	  {
	    twi_sl_update (data, TWI_SL_RCPT_SIZE);
	  }
      }
    return 0;
}
