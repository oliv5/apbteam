/* flash_at.avr.c */
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
#include "flash_at.h"
#include "modules/spi/spi.h"

enum flash_at_comp_t
{
    FLASH_AT_COMP_OK,
    FLASH_AT_COMP_NOK,
    FLASH_AT_COMP_NB
};

enum flash_at_sector_protection_t
{
    FLASH_AT_SECTOR_PROTECTION_ENABLE,
    FLASH_AT_SECTOR_PROTECTION_DISABLE,
    FLASH_AT_SECTOR_PROTECTION_NB
};

enum flash_at_page_size_t
{
    FLASH_AT_PAGE_SIZE_512,
    FLASH_AT_PAGE_SIZE_528,
    FLASH_AT_PAGE_SIZE_NB
};

struct flash_at_buffer_t
{
    /* Buffer loaded. */
    uint8_t loaded;
    /* Buffer page. */
    uint16_t page;
    /* Position in page. */
    uint16_t position;
    /* Load command. */
    uint8_t load_cmd;
    /* Store command. */
    uint8_t store_cmd;
    /* To acknowledge. */
    uint8_t toack;
};

struct flash_at_t
{
    /** Buffer 1. */
    struct flash_at_buffer_t buffer1;
    /** Buffer 2. */
    struct flash_at_buffer_t buffer2;
};

/* Flash at context. */
static struct flash_at_t flash_at;

/** Verify if the main memory page correspond to the buffer page.
 * \return  1 on success i.e. buffer == main memory page.
 */
static inline enum flash_at_comp_t
flash_at_mmp_to_buff (void)
{
    return !((flash_at_read_status () >> 6) & 0x1);
}

/** Read the sector protection status.
 * \return  1 sector protection enabled.
 */
static inline enum flash_at_sector_protection_t
flash_at_sector_protection (void)
{
    return ((flash_at_read_status () >> 1) & 0x1);
}

/** Read the page size configuration.
 * \return 1 if configured as 512 bytes page, 0 for 528 bytes.
 */
static inline enum flash_at_page_size_t
flash_at_page_size (void)
{
    return flash_at_read_status () & 0x1;
}

/** Disable flash protection.
*/
static inline void
flash_at_sector_protection_disable (void)
{
    FLASH_CS_ENABLE;
    spi_send (0x3D);
    spi_send (0x2A);
    spi_send (0x7F);
    spi_send (0x9A);
    FLASH_CS_DISABLE;
    /* FIXME: WP Disable ? */
}

void
flash_at_init (void)
{
    /* Initialise context. */
    flash_at.buffer1.loaded = 0;
    flash_at.buffer1.load_cmd = FLASH_AT_CMD_WRITE_BUFF1;
    flash_at.buffer1.store_cmd = FLASH_AT_CMD_BUFF1_STORE;
    flash_at.buffer2.loaded = 0;
    flash_at.buffer2.load_cmd = FLASH_AT_CMD_WRITE_BUFF2;
    flash_at.buffer2.store_cmd = FLASH_AT_CMD_BUFF2_STORE;
    /* Get status register value. */
    uint8_t status = flash_at_read_status ();
    /* Is flash protected ? */
    if ((status >> 1) & 0x1)
	flash_at_sector_protection_disable ();
    /* FIXME: Erase a part of the flash to use it. */
}

void
flash_at_erase (flash_erase_cmd_t cmd, uint32_t start_addr)
{
    uint16_t memory;
    switch (cmd)
      {
      case FLASH_ERASE_PAGE:
	memory = FLASH_AT_ADDR_TO_PAGE (start_addr);
	FLASH_CS_ENABLE;
	spi_send (FLASH_AT_ERASE_PAGE);
	spi_send (memory >> 3);
	spi_send ((memory << 1) & 0xff);
	spi_send (0);
	FLASH_CS_DISABLE;
	break;
      case FLASH_ERASE_BLOCK:
	memory = FLASH_AT_ADDR_TO_BLOCK (start_addr);
	FLASH_CS_ENABLE;
	spi_send (FLASH_AT_ERASE_BLOCK);
	spi_send (memory >> 3);
	spi_send ((memory << 4) & 0xff);
	spi_send (0);
	FLASH_CS_DISABLE;
	break;
      case FLASH_ERASE_SECTOR:
	memory = FLASH_AT_ADDR_TO_SECTOR (start_addr);
	FLASH_CS_ENABLE;
	spi_send (FLASH_AT_ERASE_SECTOR);
	spi_send (memory >> 3);
	spi_send (0);
	spi_send (0);
	FLASH_CS_DISABLE;
	break;
      case FLASH_ERASE_FULL:
	FLASH_CS_ENABLE;
	spi_send (0x7C);
	spi_send (0x94);
	spi_send (0x80);
	spi_send (0x9A);
	FLASH_CS_DISABLE;
	break;
      default:
	return;
      }
    while (flash_at_is_busy ());
}

void
flash_at_send_command (flash_cmd_t cmd)
{
    /* FIXME: Useful ? */
}

uint8_t
flash_at_read_status (void)
{
    uint8_t status;
    /* Read status. */
    FLASH_CS_ENABLE;
    spi_send (FLASH_AT_CMD_STATUS);
    status = spi_recv ();
    FLASH_CS_DISABLE;
    return status;
}

uint8_t
flash_at_is_busy (void)
{
    return !(flash_at_read_status () >> 7);
}

/** Store the buffer into the main memory.
 * \param  buffer  the buffer context.
 */
static inline void
flash_at_write_buffer_into_memory (struct flash_at_buffer_t *buffer)
{
    if (buffer->position == FLASH_AT_PAGE_SIZE_528)
      {
	while (flash_at_is_busy ());
	/* Store buffer in main memory. */
	FLASH_CS_ENABLE;
	/* Send opcode. */
	spi_send (buffer->store_cmd);
	/* Send page address. */
	spi_send (buffer->page >> 7);
	spi_send (buffer->page << 1);
	spi_send (0);
	FLASH_CS_DISABLE;
	buffer->loaded = 0;
	buffer->toack = 1;
      }
}

/** Write data into buffer.
 * \param  buffer  the buffer context.
 * \param  addr  the address to store the data.
 * \param  data  the data value to store.
 */
static inline void
flash_at_write_into_buffer (struct flash_at_buffer_t *buffer,
			    uint32_t addr, uint8_t data)
{
    while (flash_at_is_busy ());
    buffer->page = FLASH_AT_ADDR_TO_PAGE (addr);
    buffer->position = addr % buffer->page;
    FLASH_CS_ENABLE;
    /* Send opcode. */
    spi_send (buffer->load_cmd);
    /* Address in the buffer. */
    spi_send (0);
    spi_send (buffer->position >> 8);
    spi_send (buffer->position);
    /* Store data. */
    spi_send (data);
    FLASH_CS_DISABLE;
    buffer->position ++;
    flash_at_write_buffer_into_memory (buffer);
}

/** Acknowledge memory copy.
 * \param  buffer  the buffer context.
 * Client should only call this function after verification of the toack
 * value.
 */
static inline void
flash_at_buffer_ack (struct flash_at_buffer_t *buffer)
{
    if (flash_at_mmp_to_buff () == FLASH_AT_COMP_OK)
	buffer->toack = 0;
    else
	flash_at_write_buffer_into_memory (buffer);
}

/** Check acknowledge on the buffers.
  */
static inline void
flash_at_buffers_ack (void)
{
    if (flash_at.buffer1.toack)
	flash_at_buffer_ack (&flash_at.buffer1);
    if (flash_at.buffer2.toack)
	flash_at_buffer_ack (&flash_at.buffer2);
}

void
flash_at_write (uint32_t addr, uint8_t data)
{
    flash_at_buffers_ack ();
    if (!flash_at.buffer1.loaded
	|| (flash_at.buffer1.loaded
	    && flash_at.buffer1.page == FLASH_AT_ADDR_TO_PAGE (addr)))
      {
	flash_at.buffer1.loaded = 1;
	flash_at_write_into_buffer (&flash_at.buffer1, addr, data);
      }
    else if (!flash_at.buffer2.loaded
	     || (flash_at.buffer2.loaded
		 && flash_at.buffer2.page == FLASH_AT_ADDR_TO_PAGE (addr)))
      {
	flash_at.buffer2.loaded = 1;
	flash_at_write_into_buffer (&flash_at.buffer2, addr, data);
      }
}

uint8_t
flash_at_read (uint32_t addr)
{
    uint8_t recv;
    flash_at_buffers_ack ();
    /* Read from main memory. */
    FLASH_CS_ENABLE;
    spi_send (FLASH_AT_CMD_READ_MAIN_MEMORY);
    spi_send ((addr >> 16) & 0x1F);
    spi_send ((addr & 0xFF00) >> 8);
    spi_send (addr & 0xFF);
    recv = spi_recv ();
    FLASH_CS_DISABLE;
    return recv;
}

void
flash_at_read_array (uint32_t addr, uint8_t *buffer, uint32_t length)
{
    uint8_t i;
    flash_at_buffers_ack ();
    /* Read from main memory. */
    FLASH_CS_ENABLE;
    spi_send (FLASH_AT_CMD_READ_MAIN_MEMORY);
    spi_send ((addr >> 16) & 0x1F);
    spi_send ((addr & 0xFF00) >> 8);
    spi_send (addr & 0xFF);
    for (i = 0; i < length; i++)
	buffer[i] = spi_recv ();
    FLASH_CS_DISABLE;
}

void
flash_at_write_array (uint32_t addr, uint8_t *data, uint32_t length)
{
    uint8_t i;
    flash_at_buffers_ack ();
    /* FIXME: Use buffer to avoid multiple address access. */
    for (i = 0; i < length; i++)
	flash_at_write (addr, data[i]);
}
