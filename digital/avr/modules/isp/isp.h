#ifndef isp_h
#define isp_h
/* isp.h - ISP AVR programming. */
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

/** Results returned by ISP functions. */
enum isp_result_t
{
    /** Operation success. */
    ISP_OK,
    /** Operation failed. */
    ISP_FAILED,
    /** Operation failed on a timeout. */
    ISP_TIMEOUT,
};

/** ISP Programming mode. */
enum isp_mode_t
{
    /** Use page programming. */
    ISP_MODE_PAGE = 1,
    /** Use simple delay for word programming. */
    ISP_MODE_WORD_DELAY = 2,
    /** Use value polling if possible for word programming, fall back to delay
     * if not possible. */
    ISP_MODE_WORD_VALUE = 4,
    /** Use RDY/BSY polling for word programming. */
    ISP_MODE_WORD_RDY_BSY = 8,
    /** Use simple delay for page programming. */
    ISP_MODE_PAGE_DELAY = 16,
    /** Use value polling if possible for page programming, fall back to delay
     * if not possible. */
    ISP_MODE_PAGE_VALUE = 32,
    /** Use RDY/BSY polling for page programming. */
    ISP_MODE_PAGE_RDY_BSY = 64,
    /** Write page at end of transfer.  Used for big pages which must be
     * transfered in several packets. */
    ISP_MODE_PAGE_WRITE = 128,
};

uint8_t
isp_enter_progmode (uint8_t timeout_ms, uint8_t stab_delay_ms,
		    uint8_t cmd_exe_delay_ms, uint8_t synch_loops,
		    uint8_t byte_delay_ms, uint8_t poll_value,
		    uint8_t poll_index, uint8_t cmd[4]);

void
isp_leave_progmode (uint8_t pre_delay_ms, uint8_t post_delay_ms);

void
isp_load_address (uint32_t addr);

uint8_t
isp_chip_erase (uint8_t erase_delay_ms, uint8_t poll_method, uint8_t cmd[4]);

uint8_t
isp_program_flash_begin (uint16_t num_bytes, uint8_t mode, uint8_t delay_ms,
			 uint8_t cmd_write_mem, uint8_t cmd_write_page,
			 uint8_t cmd_read_mem, uint8_t poll[2]);

uint8_t
isp_program_eeprom_begin (uint16_t num_bytes, uint8_t mode, uint8_t delay_ms,
			  uint8_t cmd_write_mem, uint8_t cmd_write_page,
			  uint8_t cmd_read_mem, uint8_t poll[2]);

uint8_t
isp_program_continue (uint8_t *data, uint16_t size);

uint8_t
isp_program_end (void);

uint8_t
isp_read_flash_begin (uint16_t num_bytes, uint8_t cmd_read_mem);

uint8_t
isp_read_eeprom_begin (uint16_t num_bytes, uint8_t cmd_read_mem);

uint8_t
isp_read_continue (uint8_t *data, uint16_t size);

uint8_t
isp_read_end (void);

void
isp_program_misc (uint8_t cmd[4]);

uint8_t
isp_read_misc (uint8_t ret_addr, uint8_t cmd[4]);

void
isp_multi (uint8_t num_tx, uint8_t num_rx, uint8_t rx_start, uint8_t *data);

#endif /* isp_h */
