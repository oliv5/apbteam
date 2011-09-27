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


#ifdef PREPROC_TEST
#define empty_list
#define empty_list_comma
#define one_item_list a
#define one_item_list_comma ,a
#define two_items_list a, b
#define two_items_list_comma ,a ,b
#define ten_items_list a, b, c, d, e, f, g, h, i, j
#endif

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
NARG:
 0: PREPROC_NARG ()
 1: PREPROC_NARG (1)
 2: PREPROC_NARG (1, 2)
 3: PREPROC_NARG (1, 2, 3)
 10: PREPROC_NARG (ten_items_list)
 63: PREPROC_NARG (,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,, \
		   ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,)
#undef a
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

/** Reverse a list (maximum of 10 elements).
 * For example, PREPROC_LIST_REVERSE(a, b, c) will expand to c, b, a. */
#define PREPROC_LIST_REVERSE(args...) \
    PREPROC_NARG_CALL (PREPROC_LIST_REVERSE_, ## args)
#define PREPROC_LIST_REVERSE_0()
#define PREPROC_LIST_REVERSE_1(arg) arg
#define PREPROC_LIST_REVERSE_2(arg, args...) \
	PREPROC_LIST_REVERSE_1(args), arg
/* Boring details... {{{ */
#define PREPROC_LIST_REVERSE_3(arg, args...) \
	PREPROC_LIST_REVERSE_2(args), arg
#define PREPROC_LIST_REVERSE_4(arg, args...) \
	PREPROC_LIST_REVERSE_3(args), arg
#define PREPROC_LIST_REVERSE_5(arg, args...) \
	PREPROC_LIST_REVERSE_4(args), arg
#define PREPROC_LIST_REVERSE_6(arg, args...) \
	PREPROC_LIST_REVERSE_5(args), arg
#define PREPROC_LIST_REVERSE_7(arg, args...) \
	PREPROC_LIST_REVERSE_6(args), arg
#define PREPROC_LIST_REVERSE_8(arg, args...) \
	PREPROC_LIST_REVERSE_7(args), arg
#define PREPROC_LIST_REVERSE_9(arg, args...) \
	PREPROC_LIST_REVERSE_8(args), arg
#define PREPROC_LIST_REVERSE_10(arg, args...) \
	PREPROC_LIST_REVERSE_9(args), arg
/* }}} */

#ifdef PREPROC_TEST
PREPROC_LIST_REVERSE:
 : PREPROC_LIST_REVERSE (empty_list)
 a: PREPROC_LIST_REVERSE (one_item_list)
 b, a: PREPROC_LIST_REVERSE (two_items_list)
 j, i, h, g, f, e, d, c, b, a: PREPROC_LIST_REVERSE (ten_items_list)
#endif

/** Call a macro for every argument (maximum of 10).
 * For example, PREPROC_FOR(macro, a, b, c) will expand to macro(a) macro(b)
 * macro(c). */
/* First step needed to expand macro which can contain commas. */
#define PREPROC_FOR(macro, args...) \
    PREPROC_FOR_ (macro, ## args)
#define PREPROC_FOR_(macro, args...) \
    PREPROC_PASTE (PREPROC_FOR_, PREPROC_NARG (args)) (macro, args)
#define PREPROC_FOR_0(macro, dummy_arg)
#define PREPROC_FOR_1(macro, arg) macro (arg)
#define PREPROC_FOR_2(macro, arg, args...) \
    macro (arg) PREPROC_FOR_1 (macro, args)
/* Boring details... {{{ */
#define PREPROC_FOR_3(macro, arg, args...) \
    macro (arg) PREPROC_FOR_2 (macro, args)
#define PREPROC_FOR_4(macro, arg, args...) \
    macro (arg) PREPROC_FOR_3 (macro, args)
#define PREPROC_FOR_5(macro, arg, args...) \
    macro (arg) PREPROC_FOR_4 (macro, args)
#define PREPROC_FOR_6(macro, arg, args...) \
    macro (arg) PREPROC_FOR_5 (macro, args)
#define PREPROC_FOR_7(macro, arg, args...) \
    macro (arg) PREPROC_FOR_6 (macro, args)
#define PREPROC_FOR_8(macro, arg, args...) \
    macro (arg) PREPROC_FOR_7 (macro, args)
#define PREPROC_FOR_9(macro, arg, args...) \
    macro (arg) PREPROC_FOR_8 (macro, args)
#define PREPROC_FOR_10(macro, arg, args...) \
    macro (arg) PREPROC_FOR_9 (macro, args)
/* }}} */

#ifdef PREPROC_TEST
#define macro(arg) <arg>
PREPROC_FOR:
 : PREPROC_FOR (macro)
 : PREPROC_FOR (macro, empty_list)
 <a>: PREPROC_FOR (macro, one_item_list)
 <a> <b>: PREPROC_FOR (macro, two_items_list)
 <a> <b> <c> <d> <e> <f> <g> <h> <i> <j>: PREPROC_FOR (macro, ten_items_list)
If first comma is included in the list:
 : PREPROC_FOR (macro empty_list_comma)
 <a>: PREPROC_FOR (macro one_item_list_comma)
 <a> <b>: PREPROC_FOR (macro two_items_list_comma)
#undef macro
#endif

/** Call a macro for every arguments (maximum of 10) with an argument index.
 * For example, PREPROC_FOR_ENUM(macro, a, b, c) will expand to macro(0, a)
 * macro(1, b) macro(2, c). */
/* First step needed to expand macro which can contain commas. */
#define PREPROC_FOR_ENUM(macro, args...) \
    PREPROC_FOR_ENUM_ (macro, ## args)
/* Second step needed to expand args. */
#define PREPROC_FOR_ENUM_(macro, args...) \
    PREPROC_FOR_ENUM__ (macro, PREPROC_NARG (args), PREPROC_LIST_REVERSE (args))
#define PREPROC_FOR_ENUM__(macro, nargs, args...) \
    PREPROC_PASTE (PREPROC_FOR_ENUM_, nargs) (macro, args)
#define PREPROC_FOR_ENUM_0(macro, dummy_arg)
#define PREPROC_FOR_ENUM_1(macro, arg) macro (0, arg)
#define PREPROC_FOR_ENUM_2(macro, arg, args...) \
    PREPROC_FOR_ENUM_1 (macro, args) macro (1, arg)
/* Boring details... {{{ */
#define PREPROC_FOR_ENUM_3(macro, arg, args...) \
    PREPROC_FOR_ENUM_2 (macro, args) macro (2, arg)
#define PREPROC_FOR_ENUM_4(macro, arg, args...) \
    PREPROC_FOR_ENUM_3 (macro, args) macro (3, arg)
#define PREPROC_FOR_ENUM_5(macro, arg, args...) \
    PREPROC_FOR_ENUM_4 (macro, args) macro (4, arg)
#define PREPROC_FOR_ENUM_6(macro, arg, args...) \
    PREPROC_FOR_ENUM_5 (macro, args) macro (5, arg)
#define PREPROC_FOR_ENUM_7(macro, arg, args...) \
    PREPROC_FOR_ENUM_6 (macro, args) macro (6, arg)
#define PREPROC_FOR_ENUM_8(macro, arg, args...) \
    PREPROC_FOR_ENUM_7 (macro, args) macro (7, arg)
#define PREPROC_FOR_ENUM_9(macro, arg, args...) \
    PREPROC_FOR_ENUM_8 (macro, args) macro (8, arg)
#define PREPROC_FOR_ENUM_10(macro, arg, args...) \
    PREPROC_FOR_ENUM_9 (macro, args) macro (9, arg)
/* }}} */

#ifdef PREPROC_TEST
#define macro(index, arg) <index, arg>
PREPROC_FOR_ENUM:
 : PREPROC_FOR_ENUM (macro)
 : PREPROC_FOR_ENUM (macro, empty_list)
 <0, a>: PREPROC_FOR_ENUM (macro, one_item_list)
 <0, a> <1, b>: PREPROC_FOR_ENUM (macro, two_items_list)
 <0, a> <1, b> <2, c> <3, d> <4, e> <5, f> <6, g> <7, h> <8, i> <9, j>:
     PREPROC_FOR_ENUM (macro, ten_items_list)
If first comma is included in the list:
 : PREPROC_FOR_ENUM (macro empty_list_comma)
 <0, a>: PREPROC_FOR_ENUM (macro one_item_list_comma)
 <0, a> <1, b>: PREPROC_FOR_ENUM (macro two_items_list_comma)
#undef macro
#endif

#endif /* preproc_h */
