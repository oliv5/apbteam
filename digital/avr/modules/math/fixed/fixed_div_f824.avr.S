; fixed_div_f824.avr.S
; avr.math.fixed - Fixed point math module. {{{
;
; Copyright (C) 2005 Nicolas Schodet
;
; Robot APB Team/Efrei 2006.
;        Web: http:		;assos.efrei.fr/robot/
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

; Perfs:
;  1199 cycles in worse case
;  834 per second, per MHz

#define dd3 r25
#define dd2 r24
#define dd1 r23
#define dd0 r22

#define dv3 r21
#define dv2 r20
#define dv1 r19
#define dv0 r18

; r28-r29 are avoided because they should be saved.
#define rem3 r31
#define rem2 r30
#define rem1 r27
#define rem0 r26

#define __tmp_reg__ r0
#define __zero_reg__ r1

#define cnt r16

	.text
	.global fixed_div_f824
	.func fixed_div_f824
fixed_div_f824:
	push cnt
; Store sign.
	mov __tmp_reg__, dd3
	eor __tmp_reg__, dv3
; Change sign.
	sbrs dd3, 7
	rjmp 1f
	com dd3
	com dd2
	com dd1
	neg dd0
	sbci dd1, 0xff
	sbci dd2, 0xff
	sbci dd3, 0xff
1:	sbrs dv3, 7
	rjmp 2f
	com dv3
	com dv2
	com dv1
	neg dv0
	sbci dv1, 0xff
	sbci dv2, 0xff
	sbci dv3, 0xff
; Clear rem.
2:	clr rem0
	clr rem1
	movw rem2, rem0
; First loop, dropped bits.
	ldi cnt, 24
1:	;lsl dd0		; shift out dd
	lsl dd1			; do not touch dd0
	rol dd2
	rol dd3
	rol rem0		; shift in rem
	rol rem1		; 24 bits only
	rol rem2
	;rol rem3
	sub rem0, dv0		; rem -= dv
	sbc rem1, dv1
	sbc rem2, dv2
	sbc rem3, dv3
	brcc 2f			; if negative, restore rem
	add rem0, dv0
	adc rem1, dv1
	adc rem2, dv2
	adc rem3, dv3
2:	dec cnt			; test for loop
	brne 1b
; Second loop, stored bits.
	ldi cnt, 8
1:	rol dd0			; shift out dd, shift in result
	rol rem0		; shift in rem
	rol rem1
	rol rem2
	rol rem3
	sub rem0, dv0		; rem -= dv
	sbc rem1, dv1
	sbc rem2, dv2
	sbc rem3, dv3
	brcc 2f			; if negative, restore rem
	add rem0, dv0
	adc rem1, dv1
	adc rem2, dv2
	adc rem3, dv3
	clc			; result bit 0
	rjmp 3f
2:	sec			; result bit 1
3:	dec cnt			; test for loop
	brne 1b
; Last loop, stored bits, dd padding bits.
	ldi cnt, 24
1:	rol dd0			; shift out dd, shift in result
	rol dd1			; 0s come from the first loop
	rol dd2
	rol dd3
	rol rem0		; shift in rem
	rol rem1
	rol rem2
	rol rem3
	sub rem0, dv0		; rem -= dv
	sbc rem1, dv1
	sbc rem2, dv2
	sbc rem3, dv3
	brcc 2f			; if negative, restore rem
	add rem0, dv0
	adc rem1, dv1
	adc rem2, dv2
	adc rem3, dv3
	clc			; result bit 0
	rjmp 3f
2:	sec			; result bit 1
3:	dec cnt			; test for loop
	brne 1b
; Store last bit.
	rol dd0			; shift in result
	rol dd1
	rol dd2
	rol dd3
; Rounding.
	lsl rem0		; shift rem
	rol rem1
	rol rem2
	rol rem3
	cp rem0, dv0		; rem -= dv
	cpc rem1, dv1
	cpc rem2, dv2
	cpc rem3, dv3
	brcs 2f			; if positive or equal, round up
	subi dd0, 0xff
	sbci dd1, 0xff
	sbci dd2, 0xff
	sbci dd3, 0xff
2:				; else, nothing, round down
; Restore sign.
	sbrs __tmp_reg__, 7
	rjmp 1f
	com dd3
	com dd2
	com dd1
	neg dd0
	sbci dd1, 0xff
	sbci dd2, 0xff
	sbci dd3, 0xff
; Return.
1:	pop r16
	ret
	.endfunc
