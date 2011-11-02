#!/usr/local/bin/perl

open(INFILE, "Rot-Area.doc");
open(OUTFILE, ">rot-area.unx");
while (<INFILE>) {
	chomp;
	undef @Tmp;
	@Tmp = split('');
	$a = 0;
	while ($a == 0) {
		if ($Tmp[$#Tmp] eq ' ') {
			pop(@Tmp);
		} else {
			$a = 1;
		}
	}
	print OUTFILE @Tmp, "\n";
}
close INFILE;
close OUTFILE;
exit;
