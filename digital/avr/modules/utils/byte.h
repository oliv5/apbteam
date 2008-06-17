#ifndef byte_h
#define byte_h
/* byte.h */
/* avr.utils - Utilities AVR module. {{{
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

/** WARNING: Theses functions only works on little endian processor, do not
 * use them on other processors, prefer the portable bit shifts and or.  This
 * file only exist to override bad code production with avr-gcc in the bit
 * shifts idiom. */

/* Union needed for byte access. */
union _utils_byte_access
{
    uint8_t v8[4];
    uint16_t v16[2];
    uint32_t v32;
};

/** Byte packing macro, pack 4 bytes into a double word. */
extern inline uint32_t
v8_to_v32 (uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0)
{
    union _utils_byte_access ba;
    ba.v8[0] = b0;
    ba.v8[1] = b1;
    ba.v8[2] = b2;
    ba.v8[3] = b3;
    return ba.v32;
}

/** Byte packing macro, pack 2 bytes into a word. */
extern inline uint16_t
v8_to_v16 (uint8_t b1, uint8_t b0)
{
    union _utils_byte_access ba;
    ba.v8[0] = b0;
    ba.v8[1] = b1;
    return ba.v16[0];
}

/** Word packing macro, pack 2 words into a double word. */
extern inline uint32_t
v16_to_v32 (uint16_t w1, uint16_t w0)
{
    union _utils_byte_access ba;
    ba.v16[0] = w0;
    ba.v16[1] = w1;
    return ba.v32;
}

/** Byte unpacking macro, extract one specified byte from a double word. */
extern inline uint8_t
v32_to_v8 (uint32_t d, int pos)
{
    union _utils_byte_access ba;
    ba.v32 = d;
    return ba.v8[pos];
}

/** Byte unpacking macro, extract one specified byte from a word. */
extern inline uint8_t
v16_to_v8 (uint16_t w, int pos)
{
    union _utils_byte_access ba;
    ba.v16[0] = w;
    return ba.v8[pos];
}

/** Word unpacking macro, extract one specified word from a double word. */
extern inline uint16_t
v32_to_v16 (uint32_t d, int pos)
{
    union _utils_byte_access ba;
    ba.v32 = d;
    return ba.v16[pos];
}

#endif /* byte_h */
