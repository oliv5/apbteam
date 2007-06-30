// test_noise_filter.v
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

module test_noise_filter();
    parameter debug = 0;
    reg clk;
    reg rst;
    reg q;
    wire ql2, ql3, ql5;
    reg zql2, zql3, zql5;

    `include "common.v"

    // Clock generator.
    always #5 clk <= !clk;

    // Instantiation.
    noise_filter #(2) uut_size2 (clk, rst, q, ql2);
    noise_filter uut_size3 (clk, rst, q, ql3);
    noise_filter #(5) uut_size5 (clk, rst, q, ql5);

    integer i;
    time r, to;

    initial begin
	$dumpfile ("test_noise_filter.vcd");
	$dumpvars;
	clk <= 1;
	rst <= 0;
	q <= 0;
	zql2 <= 0; zql3 <= 0; zql5 <= 0;
	#2 rst <= 1;
	#6 // 2 ns before clock edge.
	// Toggle q, then draw a random delay.  Check that filtered output
	// switch at the right time.
	q <= ~q;
	repeat (1000) begin
	    #1 r = $random;
	    r = r % 8 + 1;
	    to = (r * 10) - 1;
	    if (debug)
		$display ("%t: rand %1d", $time, r);
	    fork
		// This checks the filtered output is unchanged before the
		// right clock edge, and changed after, or not changed at all
		// when the delay is too small.
		// Did not found an easy way to factorise this (tasks sample
		// input at invocation):
		begin
		    if (r > 2) begin
			#(2 * 10) assert (ql2, zql2);
			#2 assert (ql2, q);
		    end
		    else
			#(to) assert (ql2, zql2);
		    if (r >= 2)
			zql2 <= q;
		end
		begin
		    if (r > 3) begin
			#(3 * 10) assert (ql3, zql3);
			#2 assert (ql3, q);
		    end
		    else
			#(to) assert (ql3, zql3);
		    if (r >= 3)
			zql3 <= q;
		end
		begin
		    if (r > 5) begin
			#(5 * 10) assert (ql5, zql5);
			#2 assert (ql5, q);
		    end
		    else
			#(to) assert (ql5, zql5);
		    if (r >= 5)
			zql5 <= q;
		end
		begin
		    #(to) q <= ~q;
		end
	    join
	    r = 0;
	end
	$finish;
    end

endmodule
