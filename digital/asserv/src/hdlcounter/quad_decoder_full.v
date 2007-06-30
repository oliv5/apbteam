// quad_decoder_full.v - Full quadrature signal decoder.
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
//         ^  ^  ^  ^  ^  ^  ^  ^  ^  ^  ^
//
// Counting is done on any rising or falling edge.  The encoder can be seen as
// a absolute grey code encoder with two bits and four position.  When one bit
// change, it is easy to know what is the direction of the encoder knowing the
// old state.
//
// Timing constraints: one state duration should not be shorter than one clock
// period, except when changing direction.  In actual device, of course
// a security factor should be added, beware of optical encoders tolerance.

module quad_decoder_full(clk, rst, q, count);
    parameter bits = 8;
    input clk;
    input rst;
    input [1:0] q;
    output [bits-1:0] count;

    reg [bits-1:0] count;
    // Old input.
    reg [1:0] zq;

    always @(posedge clk or negedge rst) begin
	if (!rst) begin
	    count <= 0;
	    zq <= 0;
	end
	else begin
	    case ({ zq, q })
		4'b00_01, 4'b01_11, 4'b11_10, 4'b10_00:
		    count <= count + 1;
		4'b01_00, 4'b11_01, 4'b10_11, 4'b00_10:
		    count <= count - 1;
		default:
		    count <= count;
	    endcase
	    zq <= q;
	end
    end

endmodule
