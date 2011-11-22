#!/usr/bin/perl


# File list:
my @lsi = ( "adder3", "adder4", "adder5", "adder", "example",
		"maj2", "mux", "simple" );

my @gate = ( "simple2", "simple31", "simple32", "simple3",
		"simple4", "simple" );

my $cmd = "";

# Run QCA-LG for every netlist file:
foreach( @lsi ) {
	$cmd = "./target <$_.lsi >$_.lsi.block";
	print $cmd, "\n";
	system( $cmd );
}

foreach( @gate ) {
	$cmd = "./target gate <$_.gate >$_.gate.block";
	print $cmd, "\n";
	system( $cmd );
}


exit 0;
