// updown_counter.v - Up and down counter.
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

module updown_counter(clk, rst, updown, counter);
    parameter bits = 8;
    input clk;
    input rst;
    // Bit updown[1] is for up and bit updown[0] is for down.
    input [1:0] updown;
    output [bits-1:0] counter;

    reg [bits-1:0] counter;

    always @(posedge clk or negedge rst) begin
	if (!rst) begin
	    counter <= 0;
	end
	else begin
	    case (updown)
		2'b00:
		    counter <= counter;
		2'b10:
		    counter <= counter + 1;
		2'b01:
		    counter <= counter - 1;
		// Do not synthesis useless logic, 2'b11 is an undefined case.
		default:
		    counter <= { bits { 1'bX }};
	    endcase
	end
    end
endmodule
