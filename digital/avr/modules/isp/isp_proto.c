/* isp_proto.c */
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

#include "modules/utils/byte.h"

#include "isp_proto.h"
#include "isp.h"

#include <avr/pgmspace.h>

#define ISP_PROTO_CMD_SIGN_ON 0x01
#define ISP_PROTO_CMD_SET_PARAMETER 0x02
#define ISP_PROTO_CMD_GET_PARAMETER 0x03
#define ISP_PROTO_CMD_SET_DEVICE_PARAMETERS 0x04
#define ISP_PROTO_CMD_OSCCAL 0x05
#define ISP_PROTO_CMD_LOAD_ADDRESS 0x06
#define ISP_PROTO_CMD_FIRMWARE_UPGRADE 0x07

#define ISP_PROTO_CMD_ENTER_PROGMODE_ISP 0x10
#define ISP_PROTO_CMD_LEAVE_PROGMODE_ISP 0x11
#define ISP_PROTO_CMD_CHIP_ERASE_ISP 0x12
#define ISP_PROTO_CMD_PROGRAM_FLASH_ISP 0x13
#define ISP_PROTO_CMD_READ_FLASH_ISP 0x14
#define ISP_PROTO_CMD_PROGRAM_EEPROM_ISP 0x15
#define ISP_PROTO_CMD_READ_EEPROM_ISP 0x16
#define ISP_PROTO_CMD_PROGRAM_FUSE_ISP 0x17
#define ISP_PROTO_CMD_READ_FUSE_ISP 0x18
#define ISP_PROTO_CMD_PROGRAM_LOCK_ISP 0x19
#define ISP_PROTO_CMD_READ_LOCK_ISP 0x1a
#define ISP_PROTO_CMD_READ_SIGNATURE_ISP 0x1b
#define ISP_PROTO_CMD_READ_OSCCAL_ISP 0x1c
#define ISP_PROTO_CMD_SPI_MULTI 0x1d

#define ISP_PROTO_STATUS_CMD_OK 0x00

#define ISP_PROTO_STATUS_CMD_TOUT 0x80
#define ISP_PROTO_STATUS_RDY_BSY_TOUT 0x81
#define ISP_PROTO_STATUS_SET_PARAM_MISSING 0x82

#define ISP_PROTO_STATUS_CMD_FAILED 0xc0
#define ISP_PROTO_STATUS_CKSUM_ERROR 0xc1
#define ISP_PROTO_STATUS_CMD_UNKNOWN 0xc9

#define ISP_PROTO_ISP_STATUS(status) \
    ((status) == ISP_OK ? ISP_PROTO_STATUS_CMD_OK \
     : ISP_PROTO_STATUS_CMD_FAILED)

#define ISP_PROTO_ISP_STATUS_TOUT(status) \
    ((status) == ISP_OK ? ISP_PROTO_STATUS_CMD_OK \
     : (status) == ISP_TIMEOUT ? ISP_PROTO_STATUS_CMD_TOUT \
     : ISP_PROTO_STATUS_CMD_FAILED)

#define ISP_PROTO_PARAM_BUILD_NUMBER_LOW 0x80
#define ISP_PROTO_PARAM_BUILD_NUMBER_HIGH 0x81
#define ISP_PROTO_PARAM_HW_VER 0x90
#define ISP_PROTO_PARAM_SW_MAJOR 0x91
#define ISP_PROTO_PARAM_SW_MINOR 0x92
#define ISP_PROTO_PARAM_VTARGET 0x94
#define ISP_PROTO_PARAM_VADJUST 0x95
#define ISP_PROTO_PARAM_OSC_PSCALE 0x96
#define ISP_PROTO_PARAM_OSC_CMATCH 0x97
#define ISP_PROTO_PARAM_SCK_DURATION 0x98
#define ISP_PROTO_PARAM_TOPCARD_DETECT 0x9a
#define ISP_PROTO_PARAM_STATUS 0x9c
#define ISP_PROTO_PARAM_DATA 0x9d
#define ISP_PROTO_PARAM_RESET_POLARITY 0x9e
#define ISP_PROTO_PARAM_CONTROLLER_INIT 0x9f

uint8_t isp_proto_sck_duration;

static void
isp_proto_simple_answer (uint8_t *data, uint8_t status)
{
    data[1] = status;
    AC_ISP_PROTO_SEND (data, 2);
}

void
isp_proto_accept (uint8_t *data, uint16_t len)
{
    uint8_t status;
    uint16_t size;
    /* Decode command. */
    switch (data[0])
      {
      case ISP_PROTO_CMD_SIGN_ON:
	if (len != 1) break;
	data[1] = ISP_PROTO_STATUS_CMD_OK;
	data[2] = sizeof (AC_ISP_PROTO_SIGNATURE);
	memcpy_P (&data[3], PSTR (AC_ISP_PROTO_SIGNATURE),
		  sizeof (AC_ISP_PROTO_SIGNATURE));
	AC_ISP_PROTO_SEND (data, 3 + sizeof (AC_ISP_PROTO_SIGNATURE));
	return;
      case ISP_PROTO_CMD_SET_PARAMETER:
	if (len != 3) break;
	status = ISP_PROTO_STATUS_CMD_OK;
	switch (data[1])
	  {
	  case ISP_PROTO_PARAM_SCK_DURATION:
	    isp_proto_sck_duration = data[2];
	    break;
	  case ISP_PROTO_PARAM_RESET_POLARITY:
	    if (data[2] != 1)
		status = ISP_PROTO_STATUS_CMD_FAILED;
	    break;
	  default:
	    status = ISP_PROTO_STATUS_CMD_FAILED;
	    break;
	  }
	isp_proto_simple_answer (data, status);
	return;
      case ISP_PROTO_CMD_GET_PARAMETER:
	if (len != 2) break;
	status = ISP_PROTO_STATUS_CMD_OK;
	switch (data[1])
	  {
	  case ISP_PROTO_PARAM_SCK_DURATION:
	    data[2] = isp_proto_sck_duration;
	    break;
	  case ISP_PROTO_PARAM_RESET_POLARITY:
	    data[2] = 1;
	    break;
	  case ISP_PROTO_PARAM_BUILD_NUMBER_LOW:
	    data[2] = v16_to_v8 (AC_ISP_PROTO_BUILD_NUMBER, 0);
	    break;
	  case ISP_PROTO_PARAM_BUILD_NUMBER_HIGH:
	    data[2] = v16_to_v8 (AC_ISP_PROTO_BUILD_NUMBER, 1);
	    break;
	  case ISP_PROTO_PARAM_HW_VER:
	    data[2] = AC_ISP_PROTO_HW_VERSION;
	    break;
	  case ISP_PROTO_PARAM_SW_MAJOR:
	    data[2] = v16_to_v8 (AC_ISP_PROTO_SW_VERSION, 1);
	    break;
	  case ISP_PROTO_PARAM_SW_MINOR:
	    data[2] = v16_to_v8 (AC_ISP_PROTO_SW_VERSION, 0);
	    break;
	  default:
	    status = ISP_PROTO_STATUS_CMD_FAILED;
	    break;
	  }
	data[1] = status;
	AC_ISP_PROTO_SEND (data, status == ISP_PROTO_STATUS_CMD_OK ? 3 : 2);
	return;
      case ISP_PROTO_CMD_LOAD_ADDRESS:
	if (len != 5) break;
	isp_load_address (v8_to_v32 (data[1], data[2], data[3], data[4]));
	isp_proto_simple_answer (data, ISP_PROTO_STATUS_CMD_OK);
	return;
      case ISP_PROTO_CMD_ENTER_PROGMODE_ISP:
	if (len != 12) break;
	status = isp_enter_progmode (data[1], data[2], data[3], data[4],
				     data[5], data[6], data[7], &data[8]);
	isp_proto_simple_answer (data, ISP_PROTO_ISP_STATUS (status));
	return;
      case ISP_PROTO_CMD_LEAVE_PROGMODE_ISP:
	if (len != 3) break;
	isp_leave_progmode (data[1], data[2]);
	isp_proto_simple_answer (data, ISP_PROTO_STATUS_CMD_OK);
	return;
      case ISP_PROTO_CMD_CHIP_ERASE_ISP:
	if (len != 7) break;
	status = isp_chip_erase (data[1], data[2], &data[3]);
	isp_proto_simple_answer (data, ISP_PROTO_ISP_STATUS_TOUT (status));
	return;
      case ISP_PROTO_CMD_PROGRAM_FLASH_ISP:
      case ISP_PROTO_CMD_PROGRAM_EEPROM_ISP:
	if (len < 10) break;
	size = v8_to_v16 (data[1], data[2]);
	if (len != 10 + size) break;
	status = isp_program_begin (size, data[3], data[4], data[5], data[6],
				    data[7], &data[8], data[0] ==
				    ISP_PROTO_CMD_PROGRAM_FLASH_ISP);
	if (status == ISP_OK)
	    status = isp_program_continue (&data[10], size);
	if (status == ISP_OK)
	    status = isp_program_end ();
	isp_proto_simple_answer (data, ISP_PROTO_ISP_STATUS_TOUT (status));
	return;
      case ISP_PROTO_CMD_READ_FLASH_ISP:
      case ISP_PROTO_CMD_READ_EEPROM_ISP:
	if (len != 4) break;
	size = v8_to_v16 (data[1], data[2]);
	status = isp_read_begin (size, data[3], data[0] ==
				 ISP_PROTO_CMD_READ_FLASH_ISP);
	if (status == ISP_OK)
	    status = isp_read_continue (&data[2], size);
	if (status == ISP_OK)
	    status = isp_read_end ();
	if (status == ISP_OK)
	  {
	    data[1] = ISP_PROTO_STATUS_CMD_OK;
	    data[2 + size] = ISP_PROTO_STATUS_CMD_OK;
	    AC_ISP_PROTO_SEND (data, 2 + size + 1);
	  }
	else
	    isp_proto_simple_answer (data, ISP_PROTO_STATUS_CMD_FAILED);
	return;
      case ISP_PROTO_CMD_PROGRAM_FUSE_ISP:
      case ISP_PROTO_CMD_PROGRAM_LOCK_ISP:
	if (len != 5) break;
	isp_program_misc (&data[1]);
	data[1] = ISP_PROTO_STATUS_CMD_OK;
	data[2] = ISP_PROTO_STATUS_CMD_OK;
	AC_ISP_PROTO_SEND (data, 3);
	return;
      case ISP_PROTO_CMD_READ_FUSE_ISP:
      case ISP_PROTO_CMD_READ_LOCK_ISP:
      case ISP_PROTO_CMD_READ_SIGNATURE_ISP:
      case ISP_PROTO_CMD_READ_OSCCAL_ISP:
	if (len != 6) break;
	status = isp_read_misc (data[1], &data[2]);
	data[1] = ISP_PROTO_STATUS_CMD_OK;
	data[2] = status;
	data[3] = ISP_PROTO_STATUS_CMD_OK;
	AC_ISP_PROTO_SEND (data, 4);
	return;
      case ISP_PROTO_CMD_SPI_MULTI:
	if (len < 4) break;
	size = data[2];
	if (len != 4 + size) break;
	isp_multi (data[1], data[2], data[3], &data[4], &data[2]);
	data[1] = ISP_PROTO_STATUS_CMD_OK;
	data[2 + size] = ISP_PROTO_STATUS_CMD_OK;
	AC_ISP_PROTO_SEND (data, 2 + size + 1);
	return;
      default:
	/* Unknown. */
	isp_proto_simple_answer (data, ISP_PROTO_STATUS_CMD_UNKNOWN);
	return;
      }
    isp_proto_simple_answer (data, ISP_PROTO_STATUS_CMD_FAILED);
}

