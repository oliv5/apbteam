/* flash.c */
/* avr.flash - AVR Flash SPI use. {{{
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
#include "modules/flash/flash.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "sys/types.h"
#include "fcntl.h"
#include "stdlib.h"

#define FLASH_STUB_DATA_LEN (2 * sizeof (uint8_t))

struct flash_t
{
    /* The file descriptor simulating the Flash memory. */
    int file;
    /* The flash current status. */
    uint8_t status;
};
typedef struct flash_t flash_t;


/* Global declaration. */
static flash_t flash_global;

void
flash_erase (uint8_t cmd, uint32_t start_addr)
{
    const char data = 'F';
    uint8_t res;

    if (flash_global.status)
      {
	if (cmd == FLASH_ERASE_FULL)
	  {
	    uint32_t i;
	    res = 1;

	    lseek (flash_global.file, 0, SEEK_SET);
	    for (i = 0; i <= (FLASH_ADDRESS_HIGH * 2) && res != 0; i++)
		res = write (flash_global.file, &data, sizeof (char));
	  }
	else
	  {
	    uint32_t i;
	    uint32_t length;

	    switch (cmd)
	      {
	      case FLASH_ERASE_4K:
		length = 4096 * 2;
		break;
	      case FLASH_ERASE_32K:
		length = 32768 * 2;
		break;
	      case FLASH_ERASE_64K:
		length = 65536 * 2;
		break;
	      default:
		length = 0;
	      }

	    res = lseek (flash_global.file, FLASH_PAGE (start_addr), SEEK_SET);
	    for (i = 0, res = 1; i <= length && res == 1; i++)
		res = write (flash_global.file, &data, sizeof (char));
	    res = lseek (flash_global.file, FLASH_PAGE (start_addr), SEEK_SET);
	  }
      }
}

uint8_t
flash_read_status (void)
{
    return flash_global.status;
}

uint8_t
flash_init (void)
{
    uint8_t res = 0x0;
    uint8_t nb_bytes;

    memset (&flash_global, 0, sizeof (flash_t));

    /* Open the file. */
    flash_global.file = open ("flash.apb", O_CREAT | O_RDWR);

    if (flash_global.file)
      {
	lseek (flash_global.file, 0, SEEK_SET);
	nb_bytes = read (flash_global.file, &res, 1);
	lseek (flash_global.file, 0, SEEK_SET);

	flash_global.status = !(flash_global.file == -1);
      }

    return flash_global.status;
}

uint32_t
flash_first_sector (void)
{
    uint32_t addr;

    for (addr = 0x0;
	 addr < FLASH_ADDRESS_HIGH
	 && flash_read (addr) != 0xFF;
	 addr += FLASH_PAGE_SIZE);

    return addr;
}

/** convert a 4 bits value to the character associated in ASCII.
  * \param  data  the value to convert.
  * \return  the character corresponding to the data.
  */
char
flash_data_convert_to_char (uint8_t data)
{
    char res = '0';

    if (data <= 9)
	res = data + '0';
    else
	res = (data - 10) + 'A';

    return res;
}

/** convert a ASCII value to the 4 bytes value.
  * \param  data  the character to convert.
  * \return  the 4 bits value corresponding to the data.
  */
uint8_t
flash_data_convert_from_char (char data)
{
    uint8_t res = 0x0;

    if ((data >= '0') && (data <= '9'))
	res = data - '0';
    else
	res = (data + 10) - 'A';

    return res;
}

void
flash_write (uint32_t addr, uint8_t data)
{
    char car[2];

    /* Multiply per two the addr. */
    addr *= 2;

    car[0] = flash_data_convert_to_char (data & 0xF);
    car[1] = flash_data_convert_to_char (data >> 4);

    if (flash_global.status
	&& FLASH_ADDRESS_HIGH > addr)
      {
	uint8_t nb_bytes;

	/* Jump to the address requested. */
	lseek (flash_global.file, addr, SEEK_SET);
	nb_bytes = write (flash_global.file, car, 2 * sizeof (uint8_t));

	flash_global.status = !(nb_bytes == 0);
      }
}

uint8_t
flash_read (uint32_t addr)
{
    uint8_t res = 0x0;
    addr *= 2;

    if (flash_global.status
	&& FLASH_ADDRESS_HIGH > addr)
      {
	uint8_t nb_bytes = 0;
	uint8_t error_nb;
	char car[2];

	/* Jump to the address requested. */
	lseek (flash_global.file, addr, SEEK_SET);
	for (error_nb = 0; (error_nb < 3) && (nb_bytes == 0); error_nb ++)
	  {
	    nb_bytes = read (flash_global.file, car, 2 * sizeof (uint8_t));
	  }

	res = flash_data_convert_from_char (car[0]);
	res |= (flash_data_convert_from_char (car[1]) << 4);

	if (nb_bytes == 2)
	    return res;
	else
	    return 0xFF;
      }

    return res;
}

void
flash_read_array (uint32_t addr, uint8_t *buffer, uint32_t length)
{
    uint32_t i;

    if (buffer != NULL && length != 0)
      {
	for (i = 0; i < length; i++)
	  {
	    buffer[i] = flash_read (addr + i);
	  }
      }
}

void
flash_write_array (uint32_t addr, uint8_t *data, uint32_t length);
