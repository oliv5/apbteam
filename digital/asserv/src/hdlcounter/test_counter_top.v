// test_counter_top.v
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

module test_counter_top();
    parameter debug = 0;
    parameter nc = 4;
    reg clk;
    reg rst;
    reg [1:0] q[0:nc-1];
    reg oe, ale, rd, aord;
    reg [1:0] sel;
    wire [7:0] ad;
    wire wr = 1;

    `include "common.v"

    // Clock generator.
    always #5 clk <= !clk;

    // Counter top setup.
    wire [0:nc-1] quad_full = 4'b0011;
    reg [31:0] filter_size[0:nc-1];
    parameter max_filter = 7;
    initial begin
	filter_size[0] = 4;
	filter_size[1] = 4;
	filter_size[2] = 1;
	filter_size[3] = 1;
    end

    // Instantiation.
    assign ad = aord ? { 6'b0, sel } : 8'bz;
    counter_top uut (clk, rst, q[0], q[1], q[2], q[3], ale, rd, wr, ad);

    // The count variable is the true encoder position, multiplied by 32,
    // which is more than one encoder minimum period with a noise filter of
    // size 3 to satisfy decoding logic timing constraints.
    integer i;
    integer n[0:nc-1], diff[0:nc-1];
    reg [31:0] count[0:nc-1];
    reg [27:0] countdiv32[0:nc-1];
    reg [1:0] q_nat;

    wire [27:0] countdiv32_0 = countdiv32[0],
	countdiv32_1 = countdiv32[1],
	countdiv32_2 = countdiv32[2],
	countdiv32_3 = countdiv32[3];

    initial begin
	$dumpfile ("test_counter_top.vcd");
	$dumpvars (1, clk, rst, countdiv32_0, countdiv32_1, countdiv32_2,
	    countdiv32_3, uut, ale, rd, sel, ad, countassert, a);
	clk <= 1;
	rst <= 0;
	for (i = 0; i < nc; i = i + 1) begin
	    q[i] <= 0;
	    n[i] = 0;
	    count[i] = 0;
	    countdiv32[i] <= 0;
	end
	#2 rst <= 1;
	#6 // 2 ns before clock edge.
	repeat (debug ? 5000 : 100000) begin
	    #1
	    for (i = 0; i < nc; i = i + 1) begin
		if (n[i] == 0) begin
		    n[i] = $random;
		    n[i] = n[i] > 0 ? n[i] : -n[i];
		    n[i] = n[i] % 50 + 1;
		    diff[i] = $random % 2;
		    if (debug)
			$display ("%t.%1d: n %1d diff %1d", $time, i, n[i], diff[i]);
		end
		n[i] = n[i] - 1;
		count[i] = count[i] + diff[i];
		q_nat = (count[i] / 32) % 4;
		countdiv32[i] <= count[i] / 32;
		q[i] <= { q_nat[1], q_nat[0] ^ q_nat[1] };
	    end
	end
	$finish;
    end

    // This simulates a two dimension array.
    reg [27:0] countdiv32_smp[0:nc*max_filter-1];
    integer j;
    reg [27:0] countassert;
    reg [7:0] countassert8;

    wire [27:0] a = countdiv32_smp[0*max_filter+4];

    initial begin
	oe = 1;
	ale = 0;
	rd = 1;
	forever begin
	    @(posedge clk)
	    // Sample countdiv32 at rising edge.
	    for (j = 1; j < nc*max_filter; j = j + 1)
		countdiv32_smp[j] <= countdiv32_smp[j-1];
	    for (j = 0; j < nc; j = j + 1)
		countdiv32_smp[j * max_filter] <= countdiv32[j];
	    @(negedge clk)
	    // Check result *after* rising edge.
	    if (oe) begin
		countassert = countdiv32_smp[sel*max_filter + filter_size[sel] + 2];
		countassert8 = quad_full[sel] ? countassert[7:0] : countassert[9:2];
		// If equiped with a noise filter, accept a difference of 1.
		// This is more difficult to find the exact expected value (I
		// mean, without copy-paste the unit under test verbatim).
		if (filter_size[sel] > 1)
		    assert ((countassert8 - ad) | 1, 1);
		else
		    assertv8 (countassert8, ad);
	    end
	    else begin
		assertv8 (8'bz, ad);
	    end
	    // Prepare next check.
	    if (oe == 1) begin
		oe = 0;
		rd = 1;
		sel = $random & 2'b11;
		aord = 1;
		#1
		ale = 1;
		#1
		ale = 0;
		#1
		aord = 0;
	    end
	    else begin
		oe = 1;
		aord = 0;
		rd = 0;
	    end
	end
    end

endmodule
