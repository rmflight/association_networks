#!/usr/bin/perl
# Example of the use
# perl t2t.pl -f /home/k2n/GOLD/GWAS2.txt -o GWAS2_out1.txt
# t2t.pl: A script that checks whether the given input table
#                  conforms to the valid format.
#
# @written by: Byung H. Park
#
=steps
 Usage: t2t.pl -f <input filename> -o <output filename> 

 t2t.pl checks/performs the following eight steps.

 1. Tab delimited format. The number of columns is larger than 2.
 2. The first row of the data is the header, and the first column is titled "ID".
 3. “ID” colmun is unique for each row, but the rest of the columns are not.
 4. Symbol “:” is not found anywhere in the table.
 5. No blank spaces are found in the table. If found, they are replaced with single underscore "_".
 6. No duplicate columns are found in the table. 
 7. No duplicated rows are found in the table (This step is skipped).
 8. Transforming data for analysis.
    The final dimension (#row, #col) is reported too.
=cut

use strict;
use Getopt::Std;

my %args;
my $MIN_ELEMENTS = 2;

# hash map of integer to function reference.
# That is, a step to its corresponding function.
my %funcSteps = 
(
    1 => \&check_dimension,
    2 => \&check_header,
    3 => \&check_columns,
    4 => \&check_forbidden_character,
    5 => \&replace_blanks,
    6 => \&check_dup_cols,
    7 => \&check_dup_rows,
    8 => \&convert_to_data
);

# read arguments ...
getopt('f:o:', \%args);

my $file = $args{f};
my $outfile = $args{o};
my $g_separator = ",";
my $step = 0;

die "Usage:t2t.pl -f <input file> -o <output file>\n"
unless defined($file) && defined($outfile);

# check step option, if step == 0, perform batch processing...
foreach my $step (sort keys %funcSteps) {
    if ( (my $ret = ($funcSteps{$step}->($file, $outfile) == 0 ? 0 : $step)) ) {
	print "Step $step failed\n";
        exit;
    }
}

#----------------- End of Main ------------------

sub numTabs
{
    my $line = shift;
    my $pos  = 0;
    my $count = 0;
    my $new_pos = 0;

    while ( ($new_pos = index($line,"\t",$pos)) != -1 ) {
	$count++;
	$pos = $new_pos+1;
    }
    return $count;
}

sub my_split
{
    my $line = shift;
    my $length  = length($line);
    my $pos = 0;
    my $new_pos = 0;
    my @indices = ();
    my @line = ();

    while ( ($new_pos = index($line,"\t",$pos)) != -1 ) {
	push @indices, $new_pos;
	$pos = $new_pos + 1;
    }

    my $start = 0;
    for (my $idx=0; $idx < scalar @indices; $idx++) {
	my $len = $indices[$idx] - $start;
	if ( $len > 0 ) {
	    push @line, substr($line, $start, $len);
	}
	else {
	    push @line, "";
	}
        $start = $indices[$idx] + 1;
    }

    my $len = ($length - $start);

    if ( $len > 0 ) {
	push @line, substr($line, $start, $len);
    }
    else {
	push @line, "";
    }
    return @line;
}

#--------------------------------------------------------
#
# checks whether the input table has the equal number
# of tab delimited columns at every row.
#
# It reports the first row that has different number of
# columns from previous row(s).
#
#--------------------------------------------------------
sub check_dimension
{
    my $input = shift;

    my $num_rows = 1;    
    open(FILE, $input) || die "$!";
    my @line = split(/\t/, <FILE>);
    my $num_cols = scalar @line;

    while ( <FILE> ) {
	chomp;
	$num_rows++;
	my $cols = numTabs($_);

	if ( $num_cols != ($cols+1) ) {
	    print "*** dimenstion mistatch ***\n";
	    print "\t\t", $_, "\n";
	    print "\t\t", $cols, " colums found whereas ", $num_cols, " columns found in the previous row(s)\n";
	    close(FILE);
	    return -1;
	}
    }
    close(FILE);

    print "The table has $num_rows rows and $num_cols columns (including the header).\n";
    return 0;
}

#--------------------------------------------------------
#
# remove leading or trailing white spaces of each entry
# in an array.
# 
# crunch multiple blanks into one.
#
#--------------------------------------------------------
sub trim
{
    my $line = shift;
    
    for (my $i=0; $i<@$line; $i++) {
	@$line[$i] =~ s/^\s+|\s+$//g;
	@$line[$i] =~ s/\s{2,}/ /g;
    }
}

#--------------------------------------------------------
#
# checks that the first columns of the header line is 
# named "ID." Also a header should start with '#'
#
# It also checks no duplicate header columns exists.
#
#--------------------------------------------------------
sub check_header
{
    my $input = shift;
    my %map;
    my $ret = 0;

    open(FILE, $input) || die "$!";
    my $header = <FILE>;
    close(FILE);

    chomp $header;
    my @line = split(/\t/, $header);
    trim(\@line);

    if ( $line[0] eq "ID" ) {
	foreach my $col (@line){
	    if ( exists($map{$col}) ) {
		print "Column header $col already exists.\n";
		$ret = -1;
	    }
	    $map{$col} = 1;
	}
    }
    else {
	print  "The first column ID shoud be \"ID\"\n";
	$ret = -1;
    }

    return $ret;
}


#--------------------------------------------------------
# 
# checks whether each column has a unique value in every
# row. It should be that way for the first columns (i.e. ID),
# but not for the rest of the columns.
#
#--------------------------------------------------------
sub check_columns
{
    my $input = shift;
    my $count = 0;
    my %table;
    my $ret = 0;

    open(FILE, $input) || die "$!";

    while (<FILE> ) {
	chomp;
	my @line = my_split($_);
	trim(\@line);
	for (my $i=0; $i<scalar @line; $i++) {
	    $table{$i}{$line[$i]}=1;
	}
	$count++;
    }
    close(FILE);

    if ( scalar (keys %{$table{0}}) != $count ) {
	print "ID columns contains non-unique IDs\n";
	$ret = -1;
    }

    for (my $i=1; $i<scalar keys %table; $i++) {
	if ( scalar (keys %{$table{$i}}) == $count ) {
	    my $col = $i+1;
	    print "columns $col contains all unique valuess\n";
	    $ret = -1;
	}
    }

    return $ret;
}


#--------------------------------------------------------
#
# checks whether the input table contains the forbidden
# character, which is ":".
#
#--------------------------------------------------------
sub check_forbidden_character
{
    my $input = shift;
    my $count = 1;
    my $character = ":";

    open(FILE, $input) || die "$!";

    while ( <FILE> ) {
	if ( /$character/ ) {
	    my @line = split(/\t/, $_);
	    print "\"$character\" is found in the $count-th row with ID $line[0]\n";
	    close(FILE);
	    return -1;
	}
	$count++;
    }    

    close(FILE);
    return 0;
}


#--------------------------------------------------------
#
# replace any blank space(s) with "_".
#
#--------------------------------------------------------
sub replace_blanks
{
    my $input = shift;
    my $new_file = $input.".temp";

    open(FILE, $input) || die "$!";
    open(OUT, ">$new_file") || die "$!";

    while ( <FILE> ) {
	chomp;
	my @line = my_split($_);
	my @new_line = ();

	foreach my $col (@line) {
	    my @items = split(",", $col);
	    my @new_items = ();
	    trim(\@items);
	    foreach my $item (@items) {
	       $item =~ s/\s+/_/g;
	       push @new_items, $item;
	    }
	    push @new_line, join(",", @new_items);
	}
	
	print OUT join("\t", @new_line), "\n";
    }    
    close(OUT);
    close(FILE);

    rename($new_file, $input) || die "count not rename $new_file to $input.\n";

    return 0;
}


#--------------------------------------------------------
#
# checks whether the input table contains any pair of
# identical rows but possibly with different IDs.
#
# For this, it sorts the entire table rows by their columns
# values excluding IDs.
#
#--------------------------------------------------------
sub check_dup_rows
{
    my $input = shift;
    my $row  = 0;
    my @tuples;
    my $ret = 0;

    #H? For now only
    return $ret;


    # First read the entire table.
    open(FILE, $input) || die "$!";
    
    while ( <FILE> ) {
	chomp;
	$tuples[$row++] = $_;
    }
    close(FILE);

    # Second sort the table by their columns not
    # including the ID column.
    my @sorted_tuples = sort tuple_sort @tuples;

    # If any pair of two consecutive identical rows is found, print it out.
    for (my $i=1; $i<$row; $i++) {
	if ( compare_rows($sorted_tuples[$i-1], $sorted_tuples[$i]) == 0 ) {
	    print "*** Duplicate Rows ***\n";
	    print "\t\t", $sorted_tuples[$i-1], "\n";
	    print "\t\t", $sorted_tuples[$i], "\n";
	    $ret = -1;
	}
    }

    return $ret;
}


#--------------------------------------------------------
#
# creates all possible pairs of columns.
# 
# The resulting list is used in sifting duplicate columns.
# See check_dup_cols and compare_columns for detailed usage
# of the list.
#
#--------------------------------------------------------
sub init_dup
{
    my $dups = shift;
    my $dim = shift;
    my $count = 0;

    foreach my $i (0 .. $dim-1) {
	foreach my $j ($i+1 .. $dim-1) {
	    push @{$dups->{$count}}, $i;
	    push @{$dups->{$count}}, $j;
	    $count++;
	}
    }
}


#--------------------------------------------------------
#
# checks whether the input table contains any pair of 
# identical columns.
#
#--------------------------------------------------------
sub check_dup_cols
{
    my $input = shift;
    my %dup_cols;

    open(FILE, $input) || die "$!";
    my $row = <FILE>;
    chomp $row;

    my @line = split(/\t/, $row);
    my $dim = scalar @line;
    init_dup(\%dup_cols, $dim);
 
#   It's safe, but redundant to uncomment the following.
#   compare_columns(\%dup_cols, \@line);

    while ( <FILE> ) {
	# If pair(s) of identical columns still found 
	if ( scalar keys %dup_cols ) {
	    chomp;
	    @line = my_split($_);
	    trim(\@line);
	    compare_columns(\%dup_cols, \@line);
	}
	else {
	    close(FILE);
	    return 0;
	}
    }
    close(FILE);

    # The last row might have diffused all pairs of identical columns.
    if ( scalar keys %dup_cols ) {
	foreach my $dup (keys %dup_cols) {
	    print "Columns ", $dup_cols{$dup}[0], " and ", $dup_cols{$dup}[1], " are the same.\n"; 
	}
	return -1;
    }

    return 0;
}


#--------------------------------------------------------
#
# compares a list of pairs whether columns in each pair 
# have the same value. If a pair is found to have different
# values, it is removed from the list so that it won't be
# checked in subsequent trials.
#
#--------------------------------------------------------
sub compare_columns
{
    my $dups = shift;
    my $line = shift;

    foreach my $entry (keys %$dups) {
	if ( @$line[$dups->{$entry}[0]] ne @$line[$dups->{$entry}[1]] ) {
	    delete $dups->{$entry};
	}
    }
}


#--------------------------------------------------------
#
# compares two rows whether they are identical.
# The first column is not considered (ID column).
# 
#--------------------------------------------------------
sub compare_rows
{
    my $ra = shift;
    my $rb = shift;
    
    my @row_a = my_split($ra);
    my @row_b = my_split($rb);

    my $cols = scalar @row_a;

    # should skip the IDs
    for (my $i=1; $i<$cols; $i++) {
        if ( $row_a[$i] ne $row_b[$i] ) {
	    return 1;
	}
    }

    return 0;
}


#--------------------------------------------------------
#
# customized "compare" routine for sorting tuples.  
# It returns a partial order of two ordered tuples by 
# comparing each pair of columns in the given order, 
# i.e. column order.
#
#--------------------------------------------------------
sub tuple_sort
{
    my @first   = my_split($a);
    my @second  = my_split($b);
    my $compare = 0;

    my $len = scalar @first;

    foreach my $i ( 1 .. $len-1 ) {
	$compare = ( $first[$i] cmp $second[$i] );
	if ( $compare != 0 ) { 
	    return ( $compare ); 
	}
    }

    return $compare;
}


#--------------------------------------------------------
#
# transforms the input table to the final input format
# for association rule mining.
#
# 1. Remove header
# 2. Remove the ID column
# 3. Concatenate header with value, 
#      i.e. val -> column_name:val 
# 4. Replace 'tab' with single 'space.'
#
#--------------------------------------------------------
sub convert_to_data
{
    my $input    = shift;
    my $new_file = shift;
    my @new_line;
    my $count = 0;
    my $ret = 0;
    my %nb_rows;

    open(FILE, $input) || die "$!";
    open(OUT, ">$new_file") || die "$!";

    my $header = <FILE>;
    chomp $header;

    my @columns = split(/\t/, $header);
    my $dim = scalar @columns;

    while ( <FILE> ) {
	chomp;
	my @line = my_split($_);
	@new_line = ();

	my $elements = 0;

	# do not include ID column
	for (my $i=1; $i<$dim; $i++) {
	    # may have multiple items separated by ","
	    my @subitems = split("$g_separator", $line[$i]);
	    my $non_blank = 0;

	    foreach my $item (@subitems) {
		if ( $item =~ /and_([A-z0-9_\-]+)/ ) {
		    $item = $1;
		}
		push @new_line, uc($columns[$i]).":".lc($item);
		$elements++;
		$non_blank++;
	    }

	    if ( $non_blank > 0 ) {
		$nb_rows{uc($columns[$i])}++;
	    }
	}
	
	if ( $elements >= $MIN_ELEMENTS ) {
	    print OUT join(" ", @new_line), "\n";
	    $count++;
	}
    } 
    close(OUT);
    close(FILE);

    print "The table has $count rows and $dim columns.\n";

    return $ret;
}
