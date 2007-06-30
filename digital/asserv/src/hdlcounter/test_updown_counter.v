// test_updown_counter.v
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

module test_updown_counter();
    parameter debug = 0;
    reg clk;
    reg rst;
    reg [1:0] updown;
    wire [7:0] count8;
    wire [11:0] count12;
    
    `include "common.v"

    // Clock generator.
    always #5 clk <= !clk;

    // Instantiation.
    updown_counter uut_size8 (clk, rst, updown, count8);
    updown_counter #(12) uut_size12 (clk, rst, updown, count12);

    integer n, count, diff;

    initial begin
	$dumpfile ("test_updown_counter.vcd");
	$dumpvars;
	clk <= 1;
	rst <= 0;
	updown <= 0;
	count = 0;
	n = 0;
	#2 rst <= 1;
	repeat (1000) begin
	    @(negedge clk)
	    if (n == 0) begin
		n = $random;
		n = n > 0 ? n : -n;
		n = n % 16 + 1;
		diff = $random % 2;
		if (debug)
		    $display ("%t: n %1d diff %1d", $time, n, diff);
	    end
	    n = n - 1;
	    count = count + diff;
	    if (diff > 0)
		updown <= 2'b10;
	    else if (diff < 0)
		updown <= 2'b01;
	    else
		updown <= 2'b00;
	    @(posedge clk) #1
	    assertv8 (count[7:0], count8);
	    assertv12 (count[11:0], count12);
	end
	if (debug)
	    $display ("%t: count %1d", $time, count);
	$finish;
    end

endmodule
