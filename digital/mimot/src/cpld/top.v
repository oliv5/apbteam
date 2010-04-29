// top.v
// mimot - Mini motor control, with motor driver. {{{
//
// Copyright (C) 2010 Nicolas Schodet
//
// APBTeam:
//        Web: http://apbteam.org/
//      Email: team AT apbteam DOT org
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// }}}
`timescale 1ns / 1ps

module top(clk, rst, ale, rd, ad, q0, q1, i0, i1, dir0, brk0, pwm0, fs0, en0,
	in0a, in0b, dir1, brk1, pwm1, fs1, en1, in1a, in1b, led, x7);
    parameter size = 8;
    parameter lsize = 8;
    input clk;
    input rst;
    input ale, rd;
    inout [size-1:0] ad;
    input [1:0] q0, q1;
    input i0, i1;
    input dir0, brk0, pwm0, dir1, brk1, pwm1;
    input fs0, fs1;
    output en0, in0a, in0b, en1, in1a, in1b;
    output led;
    input x7;

    wire [1:0] qf0, qf1;
    wire [size-1:0] count0, count1;

    // Decode encoders outputs.
    input_latch f0[1:0] (clk, rst, q0, qf0);
    quad_decoder_div4 #(size) qd0 (clk, rst, qf0, count0);

    input_latch f1[1:0] (clk, rst, q1, qf1);
    quad_decoder_div4 #(size) qd1 (clk, rst, qf1, count1);

    reg [lsize-1:0] lcount;

    // Latch a counter when its address is given.
    always @(negedge ale or negedge rst) begin
	if (!rst)
	    lcount <= 0;
	else begin
	    lcount <=
		ad[0] ==  0 ? count0 :
		count1;
	end
    end

    // Tri-state output unless rd is active (0).
    assign ad = rd ? 8'bz : lcount;

    // Power signals.
    assign en0 = brk0;
    assign in0a = brk0 & (dir0 | !pwm0);
    assign in0b = brk0 & (!dir0 | !pwm0);

    assign en1 = brk1;
    assign in1a = brk1 & (dir1 | !pwm1);
    assign in1b = brk1 & (!dir1 | !pwm1);

endmodule
