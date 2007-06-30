// input_latch.v - Input latch to protect from input change near clock edge.
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

// Input should be latched at clock rising edge in one and only one flip-flop.
// If this latch is not done, two flip-flops connected to the input might
// receive a different value if the input switch near the clock rising edge.

module input_latch(clk, rst, q, ql);
    input clk;
    input rst;
    input q;
    output ql;

    reg ql;

    always @(posedge clk or negedge rst) begin
	if (!rst)
	    ql <= 0;
	else
	    ql <= q;
    end

endmodule
