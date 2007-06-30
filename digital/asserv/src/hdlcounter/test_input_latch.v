// test_input_latch.v
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

module test_input_latch();
    parameter debug = 0;
    reg clk;
    reg rst;
    reg q;
    wire ql;

    `include "common.v"

    // Clock generator.
    always #5 clk <= !clk;

    // Instantiation.
    input_latch uut (clk, rst, q, ql);

    // This demonstrates what could happen without a latch.  The wire qnl1 and
    // qnl2 represents the input signal after two different paths in the chip,
    // where the qnl2 path is longer.  On clock edge, q is sampled, but the
    // flip flops are incoherent.
    wire qnl1, qnl2;
    reg qnlr1, qnlr2;
    assign #1 qnl1 = q;
    assign #2 qnl2 = ~q;
    always @(posedge clk or negedge rst) begin
	if (!rst) begin
	    qnlr1 <= 0;
	    qnlr2 <= 1;
	end
	else begin
	    // Actual equation is qnlr1 = q, qnl1 simulates delay inside the
	    // chip.
	    qnlr1 <= qnl1;
	    // Actual equation is qnlr2 = ~q, qnl2 simulates a longer delay
	    // inside the chip.
	    qnlr2 <= qnl2;
	end
    end

    // Now, with latched input.
    wire ql1, ql2;
    reg qlr1, qlr2;
    assign #1 ql1 = ql;
    assign #2 ql2 = ~ql;
    always @(posedge clk or negedge rst) begin
	if (!rst) begin
	    qlr1 <= 0;
	    qlr2 <= 1;
	end
	else begin
	    qlr1 <= ql1;
	    qlr2 <= ql2;
	end
    end

    initial begin
	$dumpfile ("test_input_latch.vcd");
	$dumpvars;
	clk <= 1;
	rst <= 0;
	q <= 0;
	#2 rst <= 1;
	repeat (15) begin
	    // One more than clock period to test several switch time.
	    #11 q <= ~q;
	end
	$finish;
    end

    initial begin
	#1 forever
	    #1 assert (qlr1, ~qlr2);
    end

endmodule
