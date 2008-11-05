#ifndef trace_h
#define trace_h
/* trace.h */
/*  {{{
 *
 * Copyright (C) 2008 Nélio Laranjeiro
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
/** File based on work done by Neil Booth <NeilB at earthling dot net>
  * You can find the example on the website :
  * http://gcc.gnu.org/ml/gcc-patches/2000-11/msg00016.html
  * Thanks to him.
  */
#define PASTE_EXPAND(a, b) PASTE(a, b)

#define PASTE(a, b) a ## b

#define TRACE_ARGS_COUNT(...) \
    _TRACE_ARGS_COUNT1 ( , ##__VA_ARGS__)

#define _TRACE_ARGS_COUNT1(...) \
    _TRACE_ARGS_COUNT2 (__VA_ARGS__, 10,9,8,7,6,5,4,3,2,1,0)

#define _TRACE_ARGS_COUNT2(_ ,_0,_1,_2,_3,_4,_5,_6,_7,_8,_9, n,...) n

#define TRACE(args...) \
    PASTE_EXPAND(TRACE_PRINT, TRACE_ARGS_COUNT(args...)) (args)

#define TRACE_PRINT1(args)\
    ({TRACE_PRINT_ARG_TYPE(args);})

#define TRACE_PRINT2(arg0, arg1)\
    ({ TRACE_PRINT_ARG_TYPE(arg0);\
     TRACE_PRINT_ARG_TYPE(arg1);})

#define TRACE_PRINT3(arg0, arg1, arg2)\
    ({ TRACE_PRINT_ARG_TYPE(arg0);\
     TRACE_PRINT_ARG_TYPE(arg1);\
     TRACE_PRINT_ARG_TYPE(arg2);})

#define TRACE_PRINT4(arg0, arg1, arg2, arg3)\
    ({ TRACE_PRINT_ARG_TYPE(arg0);\
     TRACE_PRINT_ARG_TYPE(arg1);\
     TRACE_PRINT_ARG_TYPE(arg2);\
     TRACE_PRINT_ARG_TYPE(arg3);})

#define TRACE_PRINT5(arg0, arg1, arg2, arg3, arg4)\
    ({ TRACE_PRINT_ARG_TYPE(arg0);\
     TRACE_PRINT_ARG_TYPE(arg1);\
     TRACE_PRINT_ARG_TYPE(arg2);\
     TRACE_PRINT_ARG_TYPE(arg3);\
     TRACE_PRINT_ARG_TYPE(arg4);})

#define TRACE_PRINT6(arg0, arg1, arg2, arg3, arg4, arg5)\
    ({ TRACE_PRINT_ARG_TYPE(arg0);\
     TRACE_PRINT_ARG_TYPE(arg1);\
     TRACE_PRINT_ARG_TYPE(arg2);\
     TRACE_PRINT_ARG_TYPE(arg3);\
     TRACE_PRINT_ARG_TYPE(arg4);\
     TRACE_PRINT_ARG_TYPE(arg5);})

#define TRACE_PRINT_ARG_TYPE(arg)\
    do\
    {\
      if (sizeof(arg) == sizeof(uint8_t)) TRACE_PRINT_ARG_1(arg);\
      else if (sizeof(arg) == sizeof(uint16_t)) TRACE_PRINT_ARG_2(arg);\
      else if (sizeof(arg) == sizeof(uint32_t)) TRACE_PRINT_ARG_4(arg);\
    }while (0)


#define TRACE_PRINT_ARG_1(arg)\
    ({trace_print_word(arg);})

#define TRACE_PRINT_ARG_2(arg)\
    ({ trace_print_word(arg >> 8);\
     trace_print_word(arg);})

#define TRACE_PRINT_ARG_4(arg)\
    ({ trace_print_word (arg >> 24);\
     trace_print_word (arg >> 16); \
     trace_print_word (arg >> 8); \
     trace_print_word (arg);})


struct trace_t
{
    /** Flash status. */
    uint8_t flash_status;
    /** Flash address. */
    uint32_t flash_addr;
    /** Flash next sector */
    uint32_t flash_next_sector;
};
typedef struct trace_t trace_t;

/** Initialise the trace module.
  */
void
trace_init (void);

/** Print the trace.
  * \param  arg  the argument to print.
  */
void
trace_print_word (uint8_t arg);

#endif /* trace_h */
