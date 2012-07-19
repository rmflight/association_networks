Introduction
============
This directory contains two applications:
  1. anet
  2. t2t.pl

anet is a tool that extracts associations between entries in data. 
The result can be used as a network of associations from which 
clusters of associated entries can be found.


t2t.pl, given a tabular data with fixed rows and columns, checks whether it
is of the correct format and produces a converted data file suitable for
association rule learning. t2t.pl performs the following eight steps.

 1. The table is of tab delimited format. Also the number of columns is larger than 2.
 2. The first row of the data is the header, and the first column is titled "ID".
 3. “ID” colmun is unique for each row, but the rest of the columns are not.
 4. Symbol “:” is not found anywhere in the table.
 5. No blank spaces are found in the table. 
    If found, they are replaced with single underscore "_".
 6. No duplicate columns are found in the table. 
 7. No duplicated rows are found in the table (This step is skipped).
 8. Transforming data for analysis.
    The final dimension (#row, #col) is reported too.


Installation
============
anet:
For Unix users (Linux, MacOS),

cd src
make

This will create the executable "anet" under "bin" directory.
Copy it into a desired directory.

t2t.pl:
This is placed under "bin" directory. Just copy it into a desired directory. 

Usage
=====
anet:

anet --file=<input file> --method=<correlation type> --output=<output file> --by=<filtering method> --count=<filtering threshold> --threshold=<output threshold>

- input file
  input file name (or full path)

-- correlation type
  spearman (default), pearson, or cosine

- output file
  output file name (or full path)

- filtering method
  This option is used to filter out annotations whose occurrences in the data 
  do not show statistical significance. Currently the following two options 
  are supported. In both cases, the threshold value is specified by "--count" option.

  1. by_cooccur_count (default)
  2. by_item_count

- count
  This option is used to specify the threshold value for the filtering method. 
  The default is one.

- threshold
  An input to this option is the largest p-value of the output entries, 
  i.e. pairs of annotations.


t2t.pl:

t2t.pl -f <input file> -o <output file>

- input file
  input file name (or full path)

- output file
  output file name (or full path)
