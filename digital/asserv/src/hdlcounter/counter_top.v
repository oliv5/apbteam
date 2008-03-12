// counter_top.v
// hdlcounter - Incremental encoder counter on programmable logic. {{{
//
// Copyright (C) 2007 Nicolas Schodet
//
// Robot APB Team 2008.
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

module counter_top(clk, rst, q0, q1, q2, q3, ale, rd, wr, ad, i0, i1, ioa,
	ioc);
    parameter size = 8;
    input clk;
    input rst;
    input [1:0] q0, q1, q2, q3;
    input ale, rd, wr;
    inout [size-1:0] ad;
    input i0, i1;
    input [3:0] ioa, ioc;

    wire [1:0] qf0, qf1, qf2, qf3;
    wire [size-1:0] count0, count1, count2, count3;

    // Decode encoders outputs.
    noise_filter f0[1:0] (clk, rst, q0, qf0);
    quad_decoder_div4 qd0 (clk, rst, qf0, count0);

    noise_filter f1[1:0] (clk, rst, q1, qf1);
    quad_decoder_div4 qd1 (clk, rst, qf1, count1);

    input_latch f2[1:0] (clk, rst, q2, qf2);
    quad_decoder_full qd2 (clk, rst, qf2, count2);

    input_latch f3[1:0] (clk, rst, q3, qf3);
    quad_decoder_full qd3 (clk, rst, qf3, count3);

    reg [size-1:0] lcount;
    
    // Latch a counter when its address is given.
    always @(negedge ale or negedge rst) begin
	if (!rst)
	    lcount <= 0;
	else begin
	    lcount <= ad[1:0] == 0 ? count0 :
		ad[1:0] == 1 ? count1 :
		ad[1:0] == 2 ? count2 :
		count3;
	end
    end

    // Tri-state output unless rd is active (0).
    assign ad = rd ? 8'bz : lcount;

endmodule
