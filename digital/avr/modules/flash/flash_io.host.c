/* flash_io.host.c */
/* avr.flash - AVR Flash SPI use. {{{
 *
 * Copyright (C) 2009 Nelio Laranjeiro
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
#include "flash_io.host.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct flash_io_t
{
    /** Status of the flash memory. */
    uint8_t status;
    /** File to store, read the data. */
    int file;
};
static struct flash_io_t flash;

/** Deactivate the flash memory.
  */
static void
flash_io_deactivate (void)
{
    flash.status = 0;

    if (flash.file)
	close (flash.file);
}

void
flash_io_erase (uint32_t start_addr, uint32_t size)
{
    if (flash.status && size)
      {
	uint8_t data = 0xFF;
	int res;
	/* Set the stream to the correct position. */
	lseek (flash.file, start_addr, SEEK_SET);
	for (res = 1; (res != -1) && size; size--)
	    res = write (flash.file, &data, sizeof (uint8_t));
	if ((res < 0) && size)
	    flash_io_deactivate ();
      }
}

uint8_t
flash_io_init (uint32_t flash_size)
{
    memset (&flash, 0, sizeof (struct flash_io_t));
    flash.file = open ("flash.apb", O_CREAT | O_RDWR, S_IREAD |
		       S_IWRITE);
    if (flash.file > 0)
      {
	struct stat fstats;
	fstat (flash.file, &fstats);
	if ((uint32_t) fstats.st_size == flash_size)
	  {
	    /* Set the stream to the beginning of the file. */
	    lseek (flash.file, 0, SEEK_SET);
	    flash.status = 1;
	  }
	else
	  {
	    flash.status = 1;
	    flash_io_erase (flash_size, 0);
	    fstat (flash.file, &fstats);
	    if ((uint32_t) fstats.st_size != flash_size)
		flash_io_deactivate ();
	  }
      }
    else
	flash_io_deactivate ();
    return flash.status;
}

void
flash_io_write (uint32_t addr, uint8_t data)
{
    if (flash.status)
      {
	uint8_t res;
	/* Set the stream to the position of address. */
	lseek (flash.file, addr, SEEK_SET);
	res = write (flash.file, &data, sizeof (uint8_t));
	if (res == 0)
	    flash_io_deactivate ();
      }
}

uint8_t
flash_io_read (uint32_t addr)
{
    if (flash.status)
      {
	uint8_t res;
	uint8_t data;
	/* Set the stream to the position of address. */
	lseek (flash.file, addr, SEEK_SET);
	res = read (flash.file, &data, sizeof (uint8_t));
	if (res == 0)
	    flash_io_deactivate ();
	return data;
      }
    return 0xFF;
}

void
flash_io_read_array (uint32_t addr, uint8_t *buffer, uint32_t length)
{
    if (flash.status)
      {
	uint8_t res;
	uint8_t deactivate = 1;
	/* Set the stream to the position of address. */
	res = lseek (flash.file, addr, SEEK_SET);
	if (res == addr)
	  {
	    res = read (flash.file, buffer, length);
	    printf ("read: %d\n", res);
	    if (res == length)
		deactivate = 0x0;
	  }
	if (deactivate == 0x1)
	    flash_io_deactivate ();
      }
}

void
flash_io_write_array (uint32_t addr, uint8_t *data, uint32_t length)
{
    if (flash.status)
      {
	uint8_t res;
	if (sizeof (data) >= length)
	  {
	    /* Set the stream to the position required. */
	    lseek (flash.file, addr, SEEK_SET);
	    res = write (flash.file, data, length);
	    if (res != length)
		flash_io_deactivate();
	  }
	else
	    flash_io_deactivate();
      }
}
