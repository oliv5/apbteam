// counter_dummy_test.v - Dummy counter test file.
// hdlcounter - Incremental encoder counter on programmable logic. {{{
//
// Copyright (C) 2008 Nicolas Schodet
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

// This module produces four addressable counters.  The first one is
// incremented when the second one overflows, and so on...  This results in the
// first counter being 256 times slower than the second counter and so on...
//
// This comes handy when no encoder is available to test communication with
// the main processor.

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

    reg [size-1:0] count0, count1, count2, count3;

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

    // Increment counters.
    always @(posedge clk or negedge rst) begin
	if (!rst) begin
	    count0 <= 0;
	    count1 <= 0;
	    count2 <= 0;
	    count3 <= 0;
	end
	else begin
	    if (count1 == 8'hff && count2 == 8'hff && count3 == 8'hff)
		count0 <= count0 + 1;
	    if (count2 == 8'hff && count3 == 8'hff)
		count1 <= count1 + 1;
	    if (count3 == 8'hff)
		count2 <= count2 + 1;
	    count3 <= count3 + 1;
	end
    end

    // Tri-state output unless rd is active (0).
    assign ad = rd ? 8'bz : lcount;

endmodule
