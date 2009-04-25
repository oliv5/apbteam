/* isp_frame.c - AVR068 serial frames support. */
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

#include "isp_frame.h"

#define ISP_FRAME_PROTO_START 27
#define ISP_FRAME_PROTO_TOKEN 14
#define ISP_FRAME_PROTO_ANSWER_CKSUM_ERROR 0xb0
#define ISP_FRAME_PROTO_STATUS_CKSUM_ERROR 0xc1

/** State of frame decoding. */
enum isp_frame_state_t
{
    /** Start state, nothing decoded. */
    ISP_FRAME_STATE_START,
    /** After message started, waiting sequence number. */
    ISP_FRAME_STATE_WAIT_SEQ,
    /** Waiting first length byte. */
    ISP_FRAME_STATE_WAIT_LEN_MSB,
    /** Waiting second length byte. */
    ISP_FRAME_STATE_WAIT_LEN_LSB,
    /** Waiting token. */
    ISP_FRAME_STATE_WAIT_TOKEN,
    /** Reading data. */
    ISP_FRAME_STATE_DATA,
    /** Waiting checksum. */
    ISP_FRAME_STATE_WAIT_CKSUM,
};

/** Context. */
struct isp_frame_t
{
    /** Current state. */
    uint8_t state;
    /** Message sequence number. */
    uint8_t seq;
    /** Message length. */
    uint16_t len;
    /** Checksum current value. */
    uint8_t cksum;
    /** Buffer to store frame until validated. */
    uint8_t buffer[AC_ISP_FRAME_BUFFER_SIZE];
    /** Used buffer length. */
    uint8_t buffer_len;
};

/** Global context. */
static struct isp_frame_t isp_frame_global;

/** Process frame in buffer. */
static void
isp_frame_process (void);

void
isp_frame_accept_char (uint8_t c)
{
    switch (isp_frame_global.state)
      {
      case ISP_FRAME_STATE_START:
	if (c == ISP_FRAME_PROTO_START)
	  {
	    isp_frame_global.cksum = ISP_FRAME_PROTO_START;
	    isp_frame_global.state++;
	  }
	break;
      case ISP_FRAME_STATE_WAIT_SEQ:
	isp_frame_global.cksum ^= c;
	isp_frame_global.seq = c;
	isp_frame_global.state++;
	break;
      case ISP_FRAME_STATE_WAIT_LEN_MSB:
	isp_frame_global.cksum ^= c;
	isp_frame_global.len = c << 8;
	isp_frame_global.state++;
	break;
      case ISP_FRAME_STATE_WAIT_LEN_LSB:
	isp_frame_global.cksum ^= c;
	isp_frame_global.len |= c;
	isp_frame_global.buffer_len = 0;
	if (isp_frame_global.len == 0
	    || isp_frame_global.len > AC_ISP_FRAME_BUFFER_SIZE)
	    isp_frame_global.state = ISP_FRAME_STATE_START;
	else
	    isp_frame_global.state++;
	break;
      case ISP_FRAME_STATE_WAIT_TOKEN:
	if (c == ISP_FRAME_PROTO_TOKEN)
	  {
	    isp_frame_global.cksum ^= c;
	    isp_frame_global.state++;
	  }
	else
	    isp_frame_global.state = ISP_FRAME_STATE_START;
	break;
      case ISP_FRAME_STATE_DATA:
	isp_frame_global.cksum ^= c;
	isp_frame_global.buffer[isp_frame_global.buffer_len++] = c;
	if (isp_frame_global.buffer_len == isp_frame_global.len)
	    isp_frame_global.state++;
	break;
      case ISP_FRAME_STATE_WAIT_CKSUM:
	isp_frame_global.cksum ^= c;
	isp_frame_global.state = ISP_FRAME_STATE_START;
	isp_frame_process ();
	break;
      }
}

void
isp_frame_send_frame (uint8_t *data, uint16_t len)
{
    uint8_t cksum;
    AC_ISP_FRAME_SEND_CHAR (ISP_FRAME_PROTO_START);
    cksum = ISP_FRAME_PROTO_START;
    AC_ISP_FRAME_SEND_CHAR (isp_frame_global.seq);
    cksum ^= isp_frame_global.seq;
    AC_ISP_FRAME_SEND_CHAR (len >> 8);
    cksum ^= len >> 8;
    AC_ISP_FRAME_SEND_CHAR (len & 0xff);
    cksum ^= len & 0xff;
    AC_ISP_FRAME_SEND_CHAR (ISP_FRAME_PROTO_TOKEN);
    cksum ^= ISP_FRAME_PROTO_TOKEN;
    for (; len; len--)
      {
	AC_ISP_FRAME_SEND_CHAR (*data);
	cksum ^= *data;
	data++;
      }
    AC_ISP_FRAME_SEND_CHAR (cksum);
}

static void
isp_frame_process (void)
{
    if (isp_frame_global.cksum != 0)
      {
	/* Bad checksum. */
	uint8_t answer[2];
	answer[0] = ISP_FRAME_PROTO_ANSWER_CKSUM_ERROR;
	answer[1] = ISP_FRAME_PROTO_STATUS_CKSUM_ERROR;
	isp_frame_send_frame (answer, 2);
      }
    else
      {
	AC_ISP_FRAME_ACCEPT_FRAME (isp_frame_global.buffer,
				   isp_frame_global.buffer_len);
      }
}

