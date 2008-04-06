// counter_index_test.v
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

// This module sample the counter on index pulse.  The purpose is to test
// decoding.  If the decoding is working, the sampled values should always be
// distant of the total number of steps.

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

    wire [1:0] qf0;
    wire [15:0] count0;

    // Decode encoders outputs.
    noise_filter f0[1:0] (clk, rst, q0, qf0);
    quad_decoder_div4 #(16) qd0 (clk, rst, qf0, count0);

    reg [size-1:0] lcount;
    reg [15:0] sample;

    // Sample on index pulse.
    always @(posedge clk or negedge rst) begin
	if (!rst)
	    sample <= 0;
	else begin
	    if (!i0)
		sample <= count0;
	    else
		sample <= sample;
	end
    end

    // Latch a counter when its address is given.
    always @(negedge ale or negedge rst) begin
	if (!rst)
	    lcount <= 0;
	else begin
	    lcount <= ad[1:0] == 0 ? count0[15:8] :
		ad[1:0] == 1 ? count0[7:0] :
		ad[1:0] == 2 ? sample[15:8] :
		sample[7:0];
	end
    end

    // Tri-state output unless rd is active (0).
    assign ad = rd ? 8'bz : lcount;

endmodule
