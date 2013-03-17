#ifndef common_h
#define common_h
/* common.h - Common definitions and includes. */
/* avr.modules - AVR modules. {{{
 *
 * Copyright (C) 2005 Nicolas Schodet
 *
 * Robot APB Team/Efrei 2006.
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

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

#ifndef HOST

# define assert(x)

# define avr_init(argc, argv) do { } while (0)

#else /* HOST */

# ifdef NDEBUG
#  warning "you should not disable debug"
# endif

# include <assert.h>

/* Should use __assert instead of __assert_fail if not using GCC.  This is
 * left as an exercise to the reader :). */
# if __GNUC__ >= 3
#  define assert_print(expr, msg) \
  (__ASSERT_VOID_CAST (__builtin_expect (!!(expr), 1) ? 0 : \
		       (__assert_fail (msg, __FILE__, __LINE__, \
				       __ASSERT_FUNCTION), 0)))
# else
#  define assert_print(expr, msg) \
  (__ASSERT_VOID_CAST ((expr) ? 0 : \
		       (__assert_fail (msg, __FILE__, __LINE__, \
				       __ASSERT_FUNCTION), 0)))
# endif

/** Initialise host module. */
void
host_init (int argc, char **argv);

#define avr_init host_init

#endif /* HOST */

#endif /* common_h */
