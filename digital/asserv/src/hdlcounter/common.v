// common.v - Common useful test utilities.
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

    initial begin
	$timeformat (-9, 0, "", 3);
    end

    task assert;
	input a, b;
	begin
	    if (a != b) begin
		$display ("%t: assertion failled ", $time, a, " != ", b);
		if (!debug)
		    $finish;
	    end
	    else if (debug)
		$display ("%t: assertion ok ", $time, a, " == ", b);
	end
    endtask

    // How to make this works for any size?
    task assertv7;
	parameter size = 7;
	input [size-1:0] a, b;
	begin
	    if (a != b) begin
		$display ("%t: assertion failled ", $time, a, " != ", b);
		if (!debug)
		    $finish;
	    end
	    else if (debug)
		$display ("%t: assertion ok ", $time, a, " == ", b);
	end
    endtask

    task assertv8;
	parameter size = 8;
	input [size-1:0] a, b;
	begin
	    if (a != b) begin
		$display ("%t: assertion failled ", $time, a, " != ", b);
		if (!debug)
		    $finish;
	    end
	    else if (debug)
		$display ("%t: assertion ok ", $time, a, " == ", b);
	end
    endtask

    task assertv12;
	parameter size = 12;
	input [size-1:0] a, b;
	begin
	    if (a != b) begin
		$display ("%t: assertion failled ", $time, a, " != ", b);
		if (!debug)
		    $finish;
	    end
	    else if (debug)
		$display ("%t: assertion ok ", $time, a, " == ", b);
	end
    endtask

