// test_quad_decoder.v
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

module test_quad_decoder();
    parameter debug = 0;
    reg clk;
    reg rst;
    reg [1:0] q;
    wire [7:0] count_div4, count_full;

    `include "common.v"

    // Clock generator.
    always #5 clk <= !clk;

    // Instantiation.
    quad_decoder_div4 uut_div4 (clk, rst, q, count_div4);
    quad_decoder_full uut_full (clk, rst, q, count_full);

    // The count variable is the true encoder position, multiplied by 16,
    // which is more than one clock period to satisfy decoding logic timing
    // constraints.
    integer n, diff;
    reg [31:0] count;
    reg [27:0] countdiv16;
    reg [1:0] q_nat;

    initial begin
	$dumpfile ("test_quad_decoder.vcd");
	$dumpvars;
	clk <= 1;
	rst <= 0;
	q <= 0;
	n = 0;
	count = 0;
	countdiv16 <= 0;
	#2 rst <= 1;
	#6 // 2 ns before clock edge.
	repeat (debug ? 1000 : 100000) begin
	    #1
	    if (n == 0) begin
		n = $random;
		n = n > 0 ? n : -n;
		n = n % 50 + 1;
		diff = $random % 2;
		if (debug)
		    $display ("%t: n %1d diff %1d", $time, n, diff);
	    end
	    n = n - 1;
	    count = count + diff;
	    q_nat = (count / 16) % 4;
	    countdiv16 <= count / 16;
	    q <= { q_nat[1], q_nat[0] ^ q_nat[1] };
	end
	$finish;
    end

    reg [27:0] countdiv16_smp;

    initial begin
	forever begin
	    @(posedge clk)
	    // Sample countdiv16 at rising edge.
	    countdiv16_smp <= countdiv16;
	    @(negedge clk)
	    // Check result *after* rising edge.
	    assertv8 (countdiv16_smp[7:0], count_full);
	    assertv8 (countdiv16_smp[9:2], count_div4);
	end
    end

endmodule
