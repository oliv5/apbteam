// noise_filter.v - Low pass noise filter.
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

// To switch output, input should be stable for a number of cycle.  This will
// filter small spikes.  As input is latched in only one flip-flop, this
// module can be used directly on an input pin.  See input_latch.v for more
// details.

module noise_filter(clk, rst, q, qf);
    parameter size = 3;
    input clk;
    input rst;
    input q;
    output qf;

    reg qf;
    reg [size-1:0] hist;

    always @(posedge clk or negedge rst) begin
	if (!rst) begin
	    qf <= 0;
	    hist <= 0;
	end
	else begin
	    if (&hist)
		qf <= 1;
	    else if (~|hist)
		qf <= 0;
	    hist <= { hist[size-2:0], q };
	end
    end

endmodule
