/* proto_inline.c */
/*  {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2005.
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
#include "modules/utils/byte.h"
#include "preproc.h"

/** Send a command with n byte arguments. */
#define proto_sendb(cmd, args...) \
    PREPROC_PASTE (proto_send, PREPROC_NARG (args), b) (cmd, args)
/** Send a command with n word arguments. */
#define proto_sendw(cmd, args...) \
    PREPROC_PASTE (proto_send, PREPROC_NARG (args), w) (cmd, args)
/** Send a command with n double word arguments. */
#define proto_sendd(cmd, args...) \
    PREPROC_PASTE (proto_send, PREPROC_NARG (args), d) (cmd, args)

/** Send a command with no argument. */
extern inline void
proto_send0 (uint8_t cmd)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 1 byte argument. */
extern inline void
proto_send1b (uint8_t cmd, uint8_t arg0)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (arg0);
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 1 word argument. */
extern inline void
proto_send1w (uint8_t cmd, uint16_t arg0)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (v16_to_v8 (arg0, 1));
    proto_arg (v16_to_v8 (arg0, 0));
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 1 double word argument. */
extern inline void
proto_send1d (uint8_t cmd, uint32_t arg0)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (v32_to_v8 (arg0, 3));
    proto_arg (v32_to_v8 (arg0, 2));
    proto_arg (v32_to_v8 (arg0, 1));
    proto_arg (v32_to_v8 (arg0, 0));
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 2 bytes arguments. */
extern inline void
proto_send2b (uint8_t cmd, uint8_t arg0, uint8_t arg1)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (arg0);
    proto_arg (arg1);
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 2 words arguments. */
extern inline void
proto_send2w (uint8_t cmd, uint16_t arg0, uint16_t arg1)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (v16_to_v8 (arg0, 1));
    proto_arg (v16_to_v8 (arg0, 0));
    proto_arg (v16_to_v8 (arg1, 1));
    proto_arg (v16_to_v8 (arg1, 0));
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 2 double words arguments. */
extern inline void
proto_send2d (uint8_t cmd, uint32_t arg0, uint32_t arg1)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (v32_to_v8 (arg0, 3));
    proto_arg (v32_to_v8 (arg0, 2));
    proto_arg (v32_to_v8 (arg0, 1));
    proto_arg (v32_to_v8 (arg0, 0));
    proto_arg (v32_to_v8 (arg1, 3));
    proto_arg (v32_to_v8 (arg1, 2));
    proto_arg (v32_to_v8 (arg1, 1));
    proto_arg (v32_to_v8 (arg1, 0));
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 3 bytes arguments. */
extern inline void
proto_send3b (uint8_t cmd, uint8_t arg0, uint8_t arg1, uint8_t arg2)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (arg0);
    proto_arg (arg1);
    proto_arg (arg2);
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 3 words arguments. */
extern inline void
proto_send3w (uint8_t cmd, uint16_t arg0, uint16_t arg1, uint16_t arg2)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (v16_to_v8 (arg0, 1));
    proto_arg (v16_to_v8 (arg0, 0));
    proto_arg (v16_to_v8 (arg1, 1));
    proto_arg (v16_to_v8 (arg1, 0));
    proto_arg (v16_to_v8 (arg2, 1));
    proto_arg (v16_to_v8 (arg2, 0));
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 3 double words arguments. */
extern inline void
proto_send3d (uint8_t cmd, uint32_t arg0, uint32_t arg1, uint32_t arg2)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (v32_to_v8 (arg0, 3));
    proto_arg (v32_to_v8 (arg0, 2));
    proto_arg (v32_to_v8 (arg0, 1));
    proto_arg (v32_to_v8 (arg0, 0));
    proto_arg (v32_to_v8 (arg1, 3));
    proto_arg (v32_to_v8 (arg1, 2));
    proto_arg (v32_to_v8 (arg1, 1));
    proto_arg (v32_to_v8 (arg1, 0));
    proto_arg (v32_to_v8 (arg2, 3));
    proto_arg (v32_to_v8 (arg2, 2));
    proto_arg (v32_to_v8 (arg2, 1));
    proto_arg (v32_to_v8 (arg2, 0));
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 4 bytes arguments. */
extern inline void
proto_send4b (uint8_t cmd, uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t
	      arg3)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (arg0);
    proto_arg (arg1);
    proto_arg (arg2);
    proto_arg (arg3);
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 4 words arguments. */
extern inline void
proto_send4w (uint8_t cmd, uint16_t arg0, uint16_t arg1, uint16_t arg2,
	      uint16_t arg3)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (v16_to_v8 (arg0, 1));
    proto_arg (v16_to_v8 (arg0, 0));
    proto_arg (v16_to_v8 (arg1, 1));
    proto_arg (v16_to_v8 (arg1, 0));
    proto_arg (v16_to_v8 (arg2, 1));
    proto_arg (v16_to_v8 (arg2, 0));
    proto_arg (v16_to_v8 (arg3, 1));
    proto_arg (v16_to_v8 (arg3, 0));
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 4 double words arguments. */
extern inline void
proto_send4d (uint8_t cmd, uint32_t arg0, uint32_t arg1, uint32_t arg2,
	      uint32_t arg3)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (v32_to_v8 (arg0, 3));
    proto_arg (v32_to_v8 (arg0, 2));
    proto_arg (v32_to_v8 (arg0, 1));
    proto_arg (v32_to_v8 (arg0, 0));
    proto_arg (v32_to_v8 (arg1, 3));
    proto_arg (v32_to_v8 (arg1, 2));
    proto_arg (v32_to_v8 (arg1, 1));
    proto_arg (v32_to_v8 (arg1, 0));
    proto_arg (v32_to_v8 (arg2, 3));
    proto_arg (v32_to_v8 (arg2, 2));
    proto_arg (v32_to_v8 (arg2, 1));
    proto_arg (v32_to_v8 (arg2, 0));
    proto_arg (v32_to_v8 (arg3, 3));
    proto_arg (v32_to_v8 (arg3, 2));
    proto_arg (v32_to_v8 (arg3, 1));
    proto_arg (v32_to_v8 (arg3, 0));
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 5 bytes arguments. */
extern inline void
proto_send5b (uint8_t cmd, uint8_t arg0, uint8_t arg1, uint8_t arg2,
	      uint8_t arg3, uint8_t arg4)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (arg0);
    proto_arg (arg1);
    proto_arg (arg2);
    proto_arg (arg3);
    proto_arg (arg4);
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 5 words arguments. */
extern inline void
proto_send5w (uint8_t cmd, uint16_t arg0, uint16_t arg1, uint16_t arg2,
	      uint16_t arg3, uint16_t arg4)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (v16_to_v8 (arg0, 1));
    proto_arg (v16_to_v8 (arg0, 0));
    proto_arg (v16_to_v8 (arg1, 1));
    proto_arg (v16_to_v8 (arg1, 0));
    proto_arg (v16_to_v8 (arg2, 1));
    proto_arg (v16_to_v8 (arg2, 0));
    proto_arg (v16_to_v8 (arg3, 1));
    proto_arg (v16_to_v8 (arg3, 0));
    proto_arg (v16_to_v8 (arg4, 1));
    proto_arg (v16_to_v8 (arg4, 0));
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 6 bytes arguments. */
extern inline void
proto_send6b (uint8_t cmd, uint8_t arg0, uint8_t arg1, uint8_t arg2,
	      uint8_t arg3, uint8_t arg4, uint8_t arg5)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (arg0);
    proto_arg (arg1);
    proto_arg (arg2);
    proto_arg (arg3);
    proto_arg (arg4);
    proto_arg (arg5);
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 6 words arguments. */
extern inline void
proto_send6w (uint8_t cmd, uint16_t arg0, uint16_t arg1, uint16_t arg2,
	      uint16_t arg3, uint16_t arg4, uint16_t arg5)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (v16_to_v8 (arg0, 1));
    proto_arg (v16_to_v8 (arg0, 0));
    proto_arg (v16_to_v8 (arg1, 1));
    proto_arg (v16_to_v8 (arg1, 0));
    proto_arg (v16_to_v8 (arg2, 1));
    proto_arg (v16_to_v8 (arg2, 0));
    proto_arg (v16_to_v8 (arg3, 1));
    proto_arg (v16_to_v8 (arg3, 0));
    proto_arg (v16_to_v8 (arg4, 1));
    proto_arg (v16_to_v8 (arg4, 0));
    proto_arg (v16_to_v8 (arg5, 1));
    proto_arg (v16_to_v8 (arg5, 0));
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 7 bytes arguments. */
extern inline void
proto_send7b (uint8_t cmd, uint8_t arg0, uint8_t arg1, uint8_t arg2,
	      uint8_t arg3, uint8_t arg4, uint8_t arg5, uint8_t arg6)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (arg0);
    proto_arg (arg1);
    proto_arg (arg2);
    proto_arg (arg3);
    proto_arg (arg4);
    proto_arg (arg5);
    proto_arg (arg6);
    AC_PROTO_PUTC ('\r');
}

/** Send a command with 8 bytes arguments. */
extern inline void
proto_send8b (uint8_t cmd,
	      uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3,
	      uint8_t arg4, uint8_t arg5, uint8_t arg6, uint8_t arg7)
{
    AC_PROTO_PUTC ('!');
    AC_PROTO_PUTC (cmd);
    proto_arg (arg0);
    proto_arg (arg1);
    proto_arg (arg2);
    proto_arg (arg3);
    proto_arg (arg4);
    proto_arg (arg5);
    proto_arg (arg6);
    proto_arg (arg7);
    AC_PROTO_PUTC ('\r');
}

