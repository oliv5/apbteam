//  {{{
//
// Copyright (C) 2013 Nicolas Schodet
//
// APBTeam:
//        Web: http://apbteam.org/
//      Email: team AT apbteam DOT org
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// }}}
#include "lcd.hh"

#include "ucoolib/arch/arch.hh"
#include "ucoolib/utils/delay.hh"
#include <libopencm3/stm32/f4/rcc.h>

#include "ucoolib/hal/i2c/i2c.hh"
#include "ucoolib/utils/crc.hh"

static const int i2c_status_size = 3;
static const int i2c_command_size = 16;

static uint8_t i2c_seq;

/// Handle a command received by I2C.
void
i2c_handle (LCD &lcd, const char *buf, int size)
{
    // Command too small.
    if (size < 3)
        return;
    // Check CRC.
    if (ucoo::crc8_compute ((const uint8_t *) buf + 1, size - 1) != buf[0])
        return;
    // Handle sequence number.
    if (buf[1] == 0)
        // Transient command, not handled for the moment.
        return;
    if (buf[1] == i2c_seq)
        // Duplicated command.
        return;
    // OK, now handle command.
    char cmd = buf[2];
    const char *arg = &buf[3];
    int arg_nb = size - 3;
    switch (cmd)
    {
    case 'c':
        // Team color.
        if (arg_nb != 3)
            return;
        lcd.clear (LCD::color (arg[0], arg[1], arg[2]));
        break;
    default:
        // Unknown command.
        return;
    }
    // Acknowledge.
    i2c_seq = buf[1];
}

/// Poll I2C interface for commands and update status.
void
i2c_poll (LCD &lcd, ucoo::I2cSlaveDataBuffer &i2c_data)
{
    char buf[i2c_command_size];
    int size;
    // Handle incoming commands.
    while ((size = i2c_data.poll (buf, sizeof (buf))))
        i2c_handle (lcd, buf, size);
    // Update status.
    char status[i2c_status_size];
    status[1] = i2c_seq;
    status[2] = 0;
    status[0] = ucoo::crc8_compute ((const uint8_t *) &status[1],
                                    sizeof (status) - 1);
    i2c_data.update (status, sizeof (status));
}

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    rcc_peripheral_enable_clock (&RCC_AHB1ENR, RCC_AHB1ENR_IOPAEN
                                 | RCC_AHB1ENR_IOPBEN | RCC_AHB1ENR_IOPCEN);
    // I2C: B8: SCL, B9: SDA
    gpio_mode_setup (GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8 | GPIO9);
    gpio_set_output_options (GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO8 | GPIO9);
    gpio_set_af (GPIOB, GPIO_AF4, GPIO8 | GPIO9);
    ucoo::I2cHard i2c (0);
    i2c.enable ();
    ucoo::I2cSlaveDataBufferSize<i2c_status_size, i2c_command_size> i2c_data;
    i2c.register_data (0x20, i2c_data);
    // Init.
    LCD lcd;
    // Wait orders.
    while (1)
    {
        i2c_poll (lcd, i2c_data);
        ucoo::delay_ms (1);
    }
}
