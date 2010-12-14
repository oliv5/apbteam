#ifndef preproc_h
#define preproc_h
/* preproc.h - C preprocessor tools. */
/* avr.modules - AVR modules. {{{
 *
 * Copyright (C) 2010 Nicolas Schodet
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

/** Define C preprocessor tools to play black magic with the C processor.
 *
 * To run tests, give it to cpp -DPREPROC_TEST. */

/** Count the number of macro arguments. */
#define PREPROC_NARG(args...) \
    PREPROC_NARG_ (dummy, ## args)
#define PREPROC_NARG_(dummy_and_args...) \
    PREPROC_NARG__ (dummy_and_args, \
		    63, 62, 61, 60, 59, 58, 57, 56, \
		    55, 54, 53, 52, 51, 50, 49, 48, \
		    47, 46, 45, 44, 43, 42, 41, 40, \
		    39, 38, 37, 36, 35, 34, 33, 32, \
		    31, 30, 29, 28, 27, 26, 25, 24, \
		    23, 22, 21, 20, 19, 18, 17, 16, \
		    15, 14, 13, 12, 11, 10,  9,  8, \
		     7,  6,  5,  4,  3,  2,  1,  0)
#define PREPROC_NARG__(__0, __1, __2, __3, __4, __5, __6, __7, \
		       __8, __9, _10, _11, _12, _13, _14, _15, \
		       _16, _17, _18, _19, _20, _21, _22, _23, \
		       _24, _25, _26, _27, _28, _29, _30, _31, \
		       _32, _33, _34, _35, _36, _37, _38, _39, \
		       _40, _41, _42, _43, _44, _45, _46, _47, \
		       _48, _49, _50, _51, _52, _53, _54, _55, \
		       _56, _57, _58, _59, _60, _61, _62, _63, \
		       N, ...) N

#ifdef PREPROC_TEST
#define a 1, 2, 3, 4
NARG:
 0: PREPROC_NARG ()
 1: PREPROC_NARG (1)
 2: PREPROC_NARG (1, 2)
 3: PREPROC_NARG (1, 2, 3)
 4: PREPROC_NARG (a)
 63: PREPROC_NARG (,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,, \
		   ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,)
#endif

/** Paste arguments together.  Macro arguments are completely macro-expanded
 * before they are substituted into a macro body, unless they are stringified
 * or pasted with other tokens.  This macro solves this problem. */
#define PREPROC_PASTE(args...) \
    PREPROC_PASTE_2_TOP (PREPROC_PASTE_, PREPROC_NARG (args)) (args)

/* Needed to avoid recursion. */
#define PREPROC_PASTE_2_TOP(a, b) PREPROC_PASTE_2_TOP_ (a, b)
#define PREPROC_PASTE_2_TOP_(a, b) a ## b
/* Need two levels to expand macro used as arguments, see tests. */
#define PREPROC_PASTE_1(a) a
#define PREPROC_PASTE_2(a, b) a ## b
#define PREPROC_PASTE_3(a, b, c) a ## b ## c
#define PREPROC_PASTE_4(a, b, c, d) a ## b ## c ## d
#define PREPROC_PASTE_5(a, b, c, d, e) a ## b ## c ## d ## e

#ifdef PREPROC_TEST
#define a1 1
#define a2 2
#define g3 3
#define WITHOUT_PASTE(x, y) x ## y ## g3
#define WITH_PASTE(x, y) PREPROC_PASTE (x, y, g3)
#define WITH_PASTE_EXPANDONCE(x, y) PREPROC_PASTE_3 (x, y, g3)
PASTE, PASTE_EXPAND:
Without PASTE, arguments and macros are not expanded:
 a1a2g3: WITHOUT_PASTE (a1, a2)
With PASTE, arguments and macros are expanded:
 123: WITH_PASTE (a1, a2)
With only one expansion, arguments are expanded, not macros:
 12g3: WITH_PASTE_EXPANDONCE (a1, a2)
#endif

/** Call a macro whose name is computed by adding the number of arguments.
 * For example, PREPROC_NARG_CALL(macro_, 1, 2, 3) will expand to macro_3 (1,
 * 2, 3). */
#define PREPROC_NARG_CALL(macro_prefix, args...) \
    PREPROC_PASTE (macro_prefix, PREPROC_NARG (args)) (args)

#ifdef PREPROC_TEST
#define macro_0() zero
#define macro_1(a) one which is a
#define macro_2(a, b) two which are a and b
#define macro_3(a, b, c) three which are a, b and c
NARG_CALL:
 zero: PREPROC_NARG_CALL (macro_)
 one which is 11: PREPROC_NARG_CALL (macro_, 11)
 two which are 11 and 22: PREPROC_NARG_CALL (macro_, 11, 22)
 three which are 11, 22 and 33: PREPROC_NARG_CALL (macro_, 11, 22, 33)
#endif

#endif /* preproc_h */
