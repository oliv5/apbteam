; fixed_mul_f824.avr.S
; avr.math.fixed - Fixed point math module. {{{
;
; Copyright (C) 2005 Nicolas Schodet
;
; Robot APB Team/Efrei 2006.
;        Web: http://assos.efrei.fr/robot/
;      Email: robot AT efrei DOT fr
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
;
; }}}

; a: A3.A2 A1 A0
; b: B3.B2 B1 B0
; m: M3.M2 M1 M0
; 
;      . => fractional dot
; A3xB3.        |
;   |A3xB2      |
;   |  .A3xB1   |
;   |  .   A3xB0|
;   |A2xB3      |
;   |  .A2xB2   |
;   |  .   A2xB1|
;   |  .      A2xB0
;   |  .A1xB3   |
;   |  .   A1xB2|
;   |  .      A1xB1
;   |  .        |A1xB0
;   |  .   A0xB3|
;   |  .      A0xB2
;   |  .        |A0xB1
;   |  .        |   A0xB0
;      .  [- step 1 --]
;   [- step 2 --]
;
; The operation is done in two steps. The three lsb multiplications can not be
; avoided.
; All multiplications results are added together. If the result is negative,
; do the sign extension (the sbc instructions).
; Result is rounded to the nearest value.
;
; Perfs:
;  96 cycles
;  10416 per second, per MHz

#define dummy1 r25
#define dummy0 r24

; mulsu are only allowed in r16-r23.
#define a3 r23
#define a2 r22
#define a1 r21
#define a0 r20

#define b3 r19
#define b2 r18
#define b1 r17
#define b0 r16

; r23 and r22 are used for a, registers will be moved before return.
#define m3 r25
#define m2 r24
#define m1 r27
#define m0 r26

#define m1r r23
#define m0r r22

#define z r30

	.text
	.global fixed_mul_f824_asm
	.func fixed_mul_f824_asm
fixed_mul_f824_asm:
	clr z
; Low dword (>> 8, with 8 guards).
	mul a0, b1
	 movw m0, r0
	 clr m2
	 clr m3
	mul a0, b0
	 add m0, r1
	 adc m1, z
	 adc m2, z
	mul a1, b0
	 add m0, r0
	 adc m1, r1
	 adc m2, z
	mul a0, b2
	 add m1, r0
	 adc m2, r1
	 adc m3, z
	mul a1, b1
	 add m1, r0
	 adc m2, r1
	 adc m3, z
	mul a2, b0
	 add m1, r0
	 adc m2, r1
	 adc m3, z
; Rounding, m0 is dropped.
	ldi m0, 0x80
	add m1, m0
	adc m2, z
	adc m3, z
; Shift.
	movw m0, m2
; Upper word.
	mulsu b3, a2
	 movw m2, r0
	mulsu b3, a0
	 sbc m2, z
	 sbc m3, z
	 add m0, r0
	 adc m1, r1
	 adc m2, z
	 adc m3, z
	mul a1, b2
	 add m0, r0
	 adc m1, r1
	 adc m2, z
	 adc m3, z
	mul a2, b1
	 add m0, r0
	 adc m1, r1
	 adc m2, z
	 adc m3, z
	mulsu a3, b0
	 sbc m2, z
	 sbc m3, z
	 add m0, r0
	 adc m1, r1
	 adc m2, z
	 adc m3, z
	mulsu b3, a1
	 sbc m3, z
	 add m1, r0
	 adc m2, r1
	 adc m3, z
	mul a2, b2
	 add m1, r0
	 adc m2, r1
	 adc m3, z
	mulsu a3, b1
	 sbc m3, z
	 add m1, r0
	 adc m2, r1
	 adc m3, z
	mulsu a3, b2
	 add m2, r0
	 adc m3, r1
	muls a3, b3
	 add m3, r0
; Restore r1 and return.
	clr r1
	movw m0r, m0
	ret
	.endfunc
