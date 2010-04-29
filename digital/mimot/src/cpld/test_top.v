// test_top.v
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

module test_top();
    reg clk;
    reg rst;
    reg ale, rd;
    wire [7:0] ad;
    reg [1:0] q0, q1;
    reg i0, i1;
    reg dir0, brk0, pwm0, fs0, dir1, brk1, pwm1, fs1;
    wire en0, in0a, in0b, en1, in1a, in1b;
    wire led;
    reg x7;

    // Clock generator.
    always #5 clk <= !clk;

    // Instantiation.
    top uut (clk, rst, ale, rd, ad, q0, q1, i0, i1, dir0, brk0, pwm0, fs0, en0,
	in0a, in0b, dir1, brk1, pwm1, fs1, en1, in1a, in1b, led, x7);

    initial begin
	$dumpfile ("test_top.vcd");
	$dumpvars;
	clk <= 1;
	rst <= 0;
	ale <= 1;
	rd <= 1;
	q0 <= 0;
	q1 <= 0;
	i0 <= 0;
	i1 <= 0;
	dir0 <= 0;
	brk0 <= 0;
	pwm0 <= 0;
	fs0 <= 0;
	dir1 <= 0;
	brk1 <= 0;
	pwm1 <= 0;
	fs1 <= 0;
	x7 <= 0;
	#3 rst <= 1;
	#50 brk0 <= 1;
	#50
	repeat (10) begin
	    #5 pwm0 <= 1;
	    #5 pwm0 <= 0;
	end
	dir0 <= 1;
	repeat (10) begin
	    #5 pwm0 <= 1;
	    #5 pwm0 <= 0;
	end
	#50 brk1 <= 1;
	#50
	repeat (10) begin
	    #5 pwm1 <= 1;
	    #5 pwm1 <= 0;
	end
	dir1 <= 1;
	repeat (10) begin
	    #5 pwm1 <= 1;
	    #5 pwm1 <= 0;
	end
	$finish;
    end

endmodule
