/* isp.c - ISP AVR programming. */
/* avr.isp - Serial programming AVR module. {{{
 *
 * Copyright (C) 2008 Nicolas Schodet
 *
 * Inspired by STK500 programmer by Guido Socher.
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

#include "modules/utils/utils.h"

#include "isp.h"

/** ISP context. */
struct isp_t
{
    /** Current address. */
    uint16_t addr;
    /** Current extended address. */
    uint8_t ext_addr;
    /** Extended address loaded in device. */
    uint8_t last_ext_addr;
    /** Whether the device needs extended addressing. */
    uint8_t larger_than_64k;
    /** Programming start address (used for page program command). */
    uint16_t start_addr;
    /** Number of bytes left to process. */
    uint16_t bytes_left;
    /** Programming mode. */
    uint8_t mode;
    /** Programming delay if no other poll method. */
    uint8_t delay_ms;
    /** Write Program Memory or Load Page. */
    uint8_t cmd_write_mem;
    /** Write Page. */
    uint8_t cmd_write_page;
    /** Read Program Memory. */
    uint8_t cmd_read_mem;
    /** Value read until memory is programmed for value polling. */
    uint8_t poll[2];
    /** Whether the page can be polled (if almost one byte is not poll[0]).
     * This is non zero if true, with LSB = 1 for high byte. */
    uint8_t pollable;
    /** Address to use for polling. */
    uint16_t poll_addr;
    /** Whether this is a flash memory. */
    uint8_t flash;
    /** Whether addresses are words addresses. */
#define word_addr flash
};

/** Command used to load extended address. */
#define ISP_CMD_LOAD_EXTENDED_ADDRESS 0x4d
/** Bit to address high byte. */
#define ISP_CMD_HIGH_BYTE (1 << 3)
/** Extra EEPROM programming delay. */
#define ISP_EEPROM_DELAY_MS 2
/** Miscellaneous read delay. */
#define ISP_MISC_READ_DELAY_MS 5

/** Global context. */
static struct isp_t isp_global;

#define isp_spi_enable AC_ISP_SPI_ENABLE
#define isp_spi_disable AC_ISP_SPI_DISABLE
#define isp_spi_sck_pulse AC_ISP_SPI_SCK_PULSE
#define isp_spi_tx AC_ISP_SPI_TX

/** Transmit a 16 bit data. */
static void
isp_spi_tx_16 (uint16_t data)
{
    isp_spi_tx ((data >> 8) & 0xff);
    isp_spi_tx ((data >> 0) & 0xff);
}

/** Transmit a 32 bit data and return last byte received.  This is used for
 * RDY/BSY polling. */
static uint8_t
isp_spi_tx_32 (uint32_t data)
{
    isp_spi_tx ((data >> 24) & 0xff);
    isp_spi_tx ((data >> 16) & 0xff);
    isp_spi_tx ((data >> 8) & 0xff);
    return isp_spi_tx ((data >> 0) & 0xff);
}

/** Wait for the specified delay, which can be a variable (it must be a
 * constant for utils_delay_ms. */
static void
isp_delay_ms (uint8_t delay_ms)
{
    while (delay_ms--)
	utils_delay_ms (1);
}

/** Enable SPI port and enter programing mode.
 * - timeout_ms: command time-out, unused.
 * - stab_delay_ms: stabilisation delay once device is reseted and SPI
 *   initialised.
 * - cmd_exe_delay_ms: delay for the command execution.
 * - synch_loops: number of synchronisation loops (there is no start of frame
 *   in SPI).
 * - byte_delay_ms: delay between each byte.
 * - poll_value: value to read to consider the sequence is correct.
 * - poll_index: transfer index at which the poll_value must be read.
 * - cmd: command bytes to be transfered.
 * - returns: ISP_OK or ISP_FAILED if no synchronisation. */
uint8_t
isp_enter_progmode (uint8_t timeout_ms, uint8_t stab_delay_ms,
		    uint8_t cmd_exe_delay_ms, uint8_t synch_loops,
		    uint8_t byte_delay_ms, uint8_t poll_value,
		    uint8_t poll_index, const uint8_t cmd[4])
{
    uint8_t i, tmp1, tmp2;
    /* Reset context. */
    isp_global.ext_addr = 0;
    isp_global.last_ext_addr = 0xff;
    isp_global.larger_than_64k = 0;
    isp_global.bytes_left = 0;
    /* Enable SPI. */
    isp_spi_enable ();
    isp_delay_ms (stab_delay_ms);
    /* Limit the number of loops. */
    if (synch_loops > 48)
	synch_loops = 48;
    /* Minimum byte delay. */
    if (byte_delay_ms < 1)
	byte_delay_ms = 1;
    /* Synchronisation loops. */
    for (i = 0; i < synch_loops; i++)
      {
	isp_delay_ms (cmd_exe_delay_ms);
	isp_spi_tx (cmd[0]);
	isp_delay_ms (byte_delay_ms);
	isp_spi_tx (cmd[1]);
	isp_delay_ms (byte_delay_ms);
	tmp1 = isp_spi_tx (cmd[2]);
	isp_delay_ms (byte_delay_ms);
	tmp2 = isp_spi_tx (cmd[3]);
	/* Test success. */
	if (poll_index == 0
	    || (poll_index == 3 && tmp1 == poll_value)
	    || (poll_index != 3 && tmp2 == poll_value))
	    return ISP_OK;
	/* Else, new try. */
	isp_spi_sck_pulse ();
	isp_delay_ms (20);
      }
    return ISP_FAILED;
}

/** Leave programming mode and disable SPI port.
 * - pre_delay_ms: delay before disabling.
 * - post_delay_ms: delay after disabling. */
void
isp_leave_progmode (uint8_t pre_delay_ms, uint8_t post_delay_ms)
{
    isp_delay_ms (pre_delay_ms);
    isp_spi_disable ();
    isp_delay_ms (post_delay_ms);
}

/** Load programing address.
 * - addr: address to load, bit 31 means the device has more than 64k words
 *   and extended addressing should be used. */
void
isp_load_address (uint32_t addr)
{
    isp_global.addr = addr & 0xffff;
    isp_global.ext_addr = (addr >> 16) & 0xff;
    isp_global.last_ext_addr = 0xff;
    isp_global.larger_than_64k = (addr & 0x80000000) ? 1 : 0;
}

/** Chip full erase.
 * - erase_delay_ms: delay to ensure the erase is finished.
 * - poll_method: use delay (0) or RDY/BSY polling (1).
 * - cmd: chip erase command.
 * - returns: ISP_OK or ISP_TIMEOUT if using polling and it timed-out. */
uint8_t
isp_chip_erase (uint8_t erase_delay_ms, uint8_t poll_method,
		const uint8_t cmd[4])
{
    uint8_t tries;
    isp_spi_tx (cmd[0]);
    isp_spi_tx (cmd[1]);
    isp_spi_tx (cmd[2]);
    isp_spi_tx (cmd[3]);
    if (poll_method == 0)
      {
	/* Use delay. */
	isp_delay_ms (erase_delay_ms);
      }
    else
      {
	/* Use RDY/BSY command. */
	tries = 150;
	while ((isp_spi_tx_32 (0xf0000000) & 1) && tries)
	    tries--;
	if (tries == 0)
	    return ISP_TIMEOUT;
      }
    return ISP_OK;
}

/** Start a program command.  See context for parameters.
 * - num_bytes: total number of bytes to program.
 * - returns: ISP_OK or ISP_FAILED for bad parameters.
 * See context for other parameters meaning. */
uint8_t
isp_program_begin (uint16_t num_bytes, uint8_t mode, uint8_t delay_ms,
		   uint8_t cmd_write_mem, uint8_t cmd_write_page,
		   uint8_t cmd_read_mem, const uint8_t poll[2], uint8_t flash)
{
    /* Set delay bounds. */
    if (delay_ms < 4)
	delay_ms = 4;
    if (delay_ms > 32)
	delay_ms = 32;
    /* Check data size. */
    if (word_addr && (num_bytes & 1))
	return ISP_FAILED;
    /* Store parameters. */
    isp_global.start_addr = isp_global.addr;
    isp_global.bytes_left = num_bytes;
    isp_global.mode = mode;
    isp_global.delay_ms = delay_ms;
    isp_global.cmd_write_mem = cmd_write_mem;
    isp_global.cmd_write_page = cmd_write_page;
    isp_global.cmd_read_mem = cmd_read_mem;
    isp_global.poll[0] = poll[0];
    isp_global.poll[1] = poll[1];
    isp_global.pollable = 0;
    isp_global.flash = flash;
    return ISP_OK;
}

/** Start a flash memory program command.
 * - num_bytes: total number of bytes to program.
 * - returns: ISP_OK or ISP_FAILED for bad parameters.
 * See context for other parameters meaning. */
uint8_t
isp_program_flash_begin (uint16_t num_bytes, uint8_t mode, uint8_t delay_ms,
			 uint8_t cmd_write_mem, uint8_t cmd_write_page,
			 uint8_t cmd_read_mem, const uint8_t poll[2])
{
    return isp_program_begin (num_bytes, mode, delay_ms, cmd_write_mem,
			      cmd_write_page, cmd_read_mem, poll, 1);
}

/** Start a EEPROM memory program command.
 * - num_bytes: total number of bytes to program.
 * - returns: ISP_OK or ISP_FAILED for bad parameters.
 * See context for other parameters meaning. */
uint8_t
isp_program_eeprom_begin (uint16_t num_bytes, uint8_t mode, uint8_t delay_ms,
			  uint8_t cmd_write_mem, uint8_t cmd_write_page,
			  uint8_t cmd_read_mem, const uint8_t poll[2])
{
    return isp_program_begin (num_bytes, mode, delay_ms, cmd_write_mem,
			      cmd_write_page, cmd_read_mem, poll, 0);
}

/** Provide data for memory programming.  Data should be given by even sized
 * packs or loading of word addressed data will fail.
 * - returns: ISP_OK, ISP_FAILED for bad parameters or ISP_TIMEOUT if using
 *   polling and it timed-out. */
uint8_t
isp_program_continue (const uint8_t *data, uint16_t size)
{
    uint16_t i;
    uint8_t tries, read;
    /* Check size. */
    if (size > isp_global.bytes_left
	|| (isp_global.word_addr && (size & 1)))
	return ISP_FAILED;
    isp_global.bytes_left -= size;
    /* Loop on each input bytes. */
    for (i = 0; i < size; i++)
      {
	/* Check for Load Extended Address. */
	if (isp_global.larger_than_64k
	    && isp_global.last_ext_addr != isp_global.ext_addr)
	  {
	    isp_spi_tx (ISP_CMD_LOAD_EXTENDED_ADDRESS);
	    isp_spi_tx (0x00);
	    isp_spi_tx (isp_global.ext_addr);
	    isp_spi_tx (0x00);
	    isp_global.last_ext_addr = isp_global.ext_addr;
	  }
	/* Write memory/load page, using bit 3 as byte selector. */
	if (isp_global.word_addr && (i & 1))
	    isp_spi_tx (isp_global.cmd_write_mem | ISP_CMD_HIGH_BYTE);
	else
	    isp_spi_tx (isp_global.cmd_write_mem);
	isp_spi_tx_16 (isp_global.addr);
	isp_spi_tx (data[i]);
	/* EEPROM needs more delay. */
	if (!isp_global.flash)
	    isp_delay_ms (ISP_EEPROM_DELAY_MS);
	/* Poll. */
	if (!(isp_global.mode & ISP_MODE_PAGE))
	  {
	    /* Poll right now. */
	    tries = 150;
	    read = isp_global.poll[0];
	    if ((isp_global.mode & ISP_MODE_WORD_VALUE)
		&& data[i] != read)
	      {
		/* Poll by reading byte. */
		while (read != data[i] && tries)
		  {
		    if (isp_global.word_addr && (i & 1))
			isp_spi_tx (isp_global.cmd_read_mem
				    | ISP_CMD_HIGH_BYTE);
		    else
			isp_spi_tx (isp_global.cmd_read_mem);
		    isp_spi_tx_16 (isp_global.addr);
		    read = isp_spi_tx (0x00);
		    tries--;
		  }
	      }
	    else if (isp_global.mode & ISP_MODE_WORD_RDY_BSY)
	      {
		/* RDY/BSY polling. */
		while ((isp_spi_tx_32 (0xf0000000) & 1) && tries)
		    tries--;
	      }
	    else
		/* Simple delay. */
		isp_delay_ms (isp_global.delay_ms);
	    if (tries == 0)
		return ISP_TIMEOUT;
	  }
	else
	  {
	    /* Check for poll method. */
	    if (!isp_global.pollable && data[i] != isp_global.poll[0])
	      {
		isp_global.pollable = 0x02 | (i & 1);
		isp_global.poll_addr = isp_global.addr;
	      }
	  }
	/* Increment address. */
	if (!isp_global.word_addr || (i & 1))
	  {
	    isp_global.addr++;
	    if (isp_global.addr == 0)
		isp_global.ext_addr++;
	  }
      }
    return ISP_OK;
}

/** End program command.
 * - returns: ISP_OK, ISP_FAILED if too early or ISP_TIMEOUT if using polling
 *   and it timed-out. */
uint8_t
isp_program_end (void)
{
    uint8_t tries, read;
    /* Check size. */
    if (isp_global.bytes_left != 0)
	return ISP_FAILED;
    /* Write page if requested. */
    if ((isp_global.mode & (ISP_MODE_PAGE | ISP_MODE_PAGE_WRITE))
	== (ISP_MODE_PAGE | ISP_MODE_PAGE_WRITE))
      {
	/* Write page. */
	isp_spi_tx (isp_global.cmd_write_page);
	isp_spi_tx_16 (isp_global.start_addr);
	isp_spi_tx (0x00);
	/* EEPROM needs more delay. */
	if (!isp_global.flash)
	    isp_delay_ms (ISP_EEPROM_DELAY_MS);
	/* Poll. */
	tries = 150;
	read = isp_global.poll[0];
	if ((isp_global.mode & ISP_MODE_PAGE_VALUE)
	    && isp_global.pollable)
	  {
	    /* Poll by reading byte. */
	    while (read == isp_global.poll[0] && tries)
	      {
		if (isp_global.word_addr && (isp_global.pollable & 1))
		    isp_spi_tx (isp_global.cmd_read_mem
				| ISP_CMD_HIGH_BYTE);
		else
		    isp_spi_tx (isp_global.cmd_read_mem);
		isp_spi_tx_16 (isp_global.poll_addr);
		read = isp_spi_tx (0x00);
		tries--;
	      }
	  }
	else if (isp_global.mode & ISP_MODE_PAGE_RDY_BSY)
	  {
	    /* RDY/BSY polling. */
	    while ((isp_spi_tx_32 (0xf0000000) & 1) && tries)
		tries--;
	  }
	else
	    /* Simple delay. */
	    isp_delay_ms (isp_global.delay_ms);
	if (tries == 0)
	    return ISP_TIMEOUT;
      }
    return ISP_OK;
}

/** Start a read command.
 * - num_bytes: total number of bytes to program.
 * - returns: ISP_OK or ISP_FAILED for bad parameters.
 * See context for other parameters meaning. */
uint8_t
isp_read_begin (uint16_t num_bytes, uint8_t cmd_read_mem, uint8_t flash)
{
    /* Check data size. */
    if (word_addr && (num_bytes & 1))
	return ISP_FAILED;
    /* Store parameters. */
    isp_global.bytes_left = num_bytes;
    isp_global.cmd_read_mem = cmd_read_mem;
    isp_global.flash = flash;
    return ISP_OK;
}

/** Start a flash memory read command.
 * - num_bytes: total number of bytes to program.
 * - returns: ISP_OK or ISP_FAILED for bad parameters.
 * See context for other parameters meaning. */
uint8_t
isp_read_flash_begin (uint16_t num_bytes, uint8_t cmd_read_mem)
{
    return isp_read_begin (num_bytes, cmd_read_mem, 1);
}

/** Start a EEPROM memory read command.
 * - num_bytes: total number of bytes to program.
 * - returns: ISP_OK or ISP_FAILED for bad parameters.
 * See context for other parameters meaning. */
uint8_t
isp_read_eeprom_begin (uint16_t num_bytes, uint8_t cmd_read_mem)
{
    return isp_read_begin (num_bytes, cmd_read_mem, 0);
}

/** Get data from read memory.  Data should be read by even sized packs or
 * loading of word addressed data will fail.
 * - returns: ISP_OK or ISP_FAILED for bad parameters. */
uint8_t
isp_read_continue (uint8_t *data, uint16_t size)
{
    uint16_t i;
    /* Check size. */
    if (size > isp_global.bytes_left
	|| (isp_global.word_addr && (size & 1)))
	return ISP_FAILED;
    isp_global.bytes_left -= size;
    /* Loop on each bytes. */
    for (i = 0; i < size; i++)
      {
	/* Check for Load Extended Address. */
	if (isp_global.larger_than_64k
	    && isp_global.last_ext_addr != isp_global.ext_addr)
	  {
	    isp_spi_tx (ISP_CMD_LOAD_EXTENDED_ADDRESS);
	    isp_spi_tx (0x00);
	    isp_spi_tx (isp_global.ext_addr);
	    isp_spi_tx (0x00);
	    isp_global.last_ext_addr = isp_global.ext_addr;
	  }
	/* Read memory, using bit 3 as byte selector. */
	if (isp_global.word_addr && (i & 1))
	    isp_spi_tx (isp_global.cmd_read_mem | ISP_CMD_HIGH_BYTE);
	else
	    isp_spi_tx (isp_global.cmd_read_mem);
	isp_spi_tx_16 (isp_global.addr);
	data[i] = isp_spi_tx (0x00);
	/* Increment address. */
	if (!isp_global.word_addr || (i & 1))
	  {
	    isp_global.addr++;
	    if (isp_global.addr == 0)
		isp_global.ext_addr++;
	  }
      }
    return ISP_OK;
}

/** End read command.
 * - returns: ISP_OK or ISP_FAILED if too early. */
uint8_t
isp_read_end (void)
{
    /* Check size. */
    if (isp_global.bytes_left != 0)
	return ISP_FAILED;
    return ISP_OK;
}

/** Program miscellaneous memory (fuse, lock).
 * - cmd: program command. */
void
isp_program_misc (const uint8_t cmd[4])
{
    uint8_t i;
    for (i = 0; i < 4; i++)
	isp_spi_tx (cmd[i]);
}

/** Read miscellaneous memory (fuse, lock, signature, osccal).
 * - ret_addr: transfer index at which the return value must be read.
 * - cmd: read command. */
uint8_t
isp_read_misc (uint8_t ret_addr, const uint8_t cmd[4])
{
    uint8_t i, read = 0, tmp;
    for (i = 0; i < 4; i++)
      {
	tmp = isp_spi_tx (cmd[i]);
	if (i == ret_addr)
	    read = tmp;
	isp_delay_ms (ISP_MISC_READ_DELAY_MS);
      }
    return read;
}

/** Generic SPI access.
 * - num_tx: number of bytes to transmit.
 * - num_rx: number of bytes to receive.
 * - rx_start: start reception after this number of transmitted bytes.
 * - dout: buffer to read sent bytes.
 * - din: buffer to write received bytes.
 * Limitation: no support for doing this in several chunks as memory
 * programing and reading. */
void
isp_multi (uint8_t num_tx, uint8_t num_rx, uint8_t rx_start,
	   const uint8_t *dout, uint8_t *din)
{
    uint8_t in, out;
    while (num_tx || num_rx)
      {
	out = 0;
	if (num_tx)
	  {
	    out = *dout++;
	    num_tx--;
	  }
	in = isp_spi_tx (out);
	if (rx_start)
	    rx_start--;
	else if (num_rx)
	  {
	    *din++ = in;
	    num_rx--;
	  }
      }
}

