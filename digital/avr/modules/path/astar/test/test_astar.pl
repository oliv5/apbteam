#!/usr/bin/perl
use strict;
use warnings;

my @tests = (
    [ 'iX___',
      '_X_X_',
      '___Xg' ],
    [ '0X_4_',
      '1X3X5',
      '_2_X6' ],
    );

while (@tests)
{
    my @in = @{shift @tests};
    my @out = @{shift @tests};
    open OUT, "./test_astar.host @in|" or die;
    my @r = grep s{^// }{}, <OUT>;
    chomp @r;
    die unless "@r" eq "@out";
}
