/* flash.host.c */
/* avr.flash - AVR Flash SPI use. {{{
 *
 * Copyright (C) 2009 Nélio Laranjeiro
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
#include "flash.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct flash_t
{
    /** Status of the flash memory. */
    uint8_t status;
    /** File to store, read the data. */
    int file;
};
static struct flash_t flash_global;

static void
flash_deactivate (void)
{
    flash_global.status = 0;

    if (flash_global.file)
	close (flash_global.file);
}

void
flash_send_command (uint8_t cmd) {}

void
flash_erase (uint8_t cmd, uint32_t start_addr)
{
    if (flash_global.status)
      {
	uint32_t length;
	switch (cmd)
	  {
	  case FLASH_ERASE_4K:
	    length = 4096;
	    break;
	  case FLASH_ERASE_32K:
	    length = 32768;
	    break;
	  case FLASH_ERASE_64K:
	    length = 65535;
	    break;
	  case FLASH_ERASE_FULL:
	    length = FLASH_SIZE;
	    break;
	  default:
	    length = 0;
	  }

	if (length)
	  {
	    uint8_t data = 0xFF;
	    int res;

	    /* Set the stream to the correct position. */
	    lseek (flash_global.file, start_addr, SEEK_SET);
	    for (res = 1; (res != -1) && length; length --)
		res = write (flash_global.file, &data, sizeof (uint8_t));

	    if ((res < 0) && length)
	      {
		flash_deactivate ();
	      }
	  }
      }
}

uint8_t
flash_read_status (void)
{
    return 0x0;
}

uint8_t
flash_init (void)
{
    memset (&flash_global, 0, sizeof (struct flash_t));

    flash_global.file = open ("flash.apb", O_CREAT | O_RDWR, S_IREAD |
			      S_IWRITE);
    if (flash_global.file > 0)
      {
	struct stat fstats;
	fstat (flash_global.file, &fstats);

	if (fstats.st_size == FLASH_SIZE)
	  {
	    /* Set the stream to the beginning of the file. */
	    lseek (flash_global.file, 0, SEEK_SET);
	    flash_global.status = 1;
	  }
	else
	  {
	    flash_global.status = 1;
	    flash_erase (FLASH_ERASE_FULL, 0);
	    fstat (flash_global.file, &fstats);
	    if (fstats.st_size != FLASH_SIZE)
		flash_deactivate ();
	  }
      }
    else
	flash_deactivate ();

    return flash_global.status;
}

void
flash_write (uint32_t addr, uint8_t data)
{
    if (flash_global.status)
      {
	uint8_t res;

	/* Set the stream to the position of address. */
	lseek (flash_global.file, addr, SEEK_SET);
	res = write (flash_global.file, &data, sizeof (uint8_t));

	if (res == 0)
	    flash_deactivate ();
      }
}

uint8_t
flash_read (uint32_t addr)
{
    if (flash_global.status)
      {
	uint8_t res;
	uint8_t data;

	/* Set the stream to the position of address. */
	lseek (flash_global.file, addr, SEEK_SET);
	res = read (flash_global.file, &data, sizeof (uint8_t));

	if (res == 0)
	    flash_global.status = 0;

	return data;
      }

    return 0xff;
}

void
flash_read_array (uint32_t addr, uint8_t *buffer, uint32_t length)
{
    if (flash_global.status)
      {
	uint32_t nb;
	int8_t res;
	/* Set the stream to the position required. */
	res = lseek (flash_global.file, addr, SEEK_SET);
	if (res == -1)
	  {
	    flash_deactivate();
	    return;
	  }

	nb = read (flash_global.file, buffer, length);

	if (nb != length)
	    flash_deactivate();
      }
}

void
flash_write_array (uint32_t addr, uint8_t *data, uint32_t length)
{
    if (flash_global.status)
      {
	uint8_t res;

	if (sizeof (data) >= length)
	  {
	    /* Set the stream to the position required. */
	    lseek (flash_global.file, addr, SEEK_SET);
	    res = write (flash_global.file, data, length);

	    if (res != length)
		flash_deactivate();
	  }
	else
	    flash_deactivate();
      }
}
