// quad_decoder_div4.v - One out of four quadrature signal decoder.
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

// Here is the signal:
//
// q[0]: _____------______------______----
// q[1]: --______------______------______-
//         ^           ^           ^
//
// Counting is always done at the same point in order not to count false
// steps.  When the encoder signal is going forward (to the right on the
// drawing), counting is done on the rising edge of q[1].  When the encoder is
// going backward, counting is done on the falling edge of q[1].  Actually,
// this is the same edge, but seen from an different point of view (forward or
// backward).  Counting is only done when q[0] is low.
//
// Timing constraints: one state duration should not be shorter than one clock
// period, except when changing direction.  In actual device, of course
// a security factor should be added, beware of optical encoders tolerance.

module quad_decoder_div4(clk, rst, q, count);
    parameter bits = 8;
    input clk;
    input rst;
    input [1:0] q;
    output [bits-1:0] count;

    reg [bits-1:0] count;
    // Old input, only one channel to remember.
    reg zq1;

    always @(posedge clk or negedge rst) begin
	if (!rst) begin
	    count <= 0;
	    zq1 <= 0;
	end
	else begin
	    case ({ q[0], zq1, q[1] })
		// 1 to 0 transition on q[1], when q[0] is 0.
		2'b0_1_0:
		    count <= count + 1;
		// 0 to 1 transition on q[1], when q[0] is 0.
		2'b0_0_1:
		    count <= count - 1;
		default:
		    count <= count;
	    endcase
	    zq1 <= q[1];
	end
    end

endmodule
