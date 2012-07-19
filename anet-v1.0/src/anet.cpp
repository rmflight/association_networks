/**
anet extracts association between items from transaction-like input data.
Copyright (C) 2012 Byung H. Park, Oak Ridge National Laboratory

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <iostream>
#include <stdlib.h>
#include <getopt.h>
#include <string>
#include "anet.h"
#include <algorithm>
     
/* Flag set by ‘--verbose’. */
static int verbose_flag;

string name;
string output_file;
string method = "spearman";
string normalization = "";
string threshold_type="by_item_count";
double threshold = 1.0;
int    threshold_count=0;

bool greaterThan(const Coefficient& a, const Coefficient& b)
{
  return a.r > b.r;
}

void usage()
{
    cout << "anet --file=<input file> --method=<default:spearman> --by=<by_item_count (default) or by_cooccur_count> --threshold=<default:1.0> --count=<count value> --output=<output file name>\n";
}


int parse_args (int argc, char **argv)
{
    int c;
    while (1) {
	static struct option long_options[] =
	    {
		{"verbose", no_argument,       &verbose_flag, 1},
		{"brief",   no_argument,       &verbose_flag, 0},
		{"file",  required_argument, 0, 'f'},
		{"output",  required_argument, 0, 'o'},
		{"by", required_argument, 0, 'b'},
		{"count", required_argument, 0, 'c'},
		{"threshold", required_argument, 0, 't'},
		{"method",  required_argument, 0, 'm'}
	    };
	   
	/* getopt_long stores the option index here. */
	int option_index = 0;
     
	c = getopt_long (argc, argv, "f:c:t:n:m:b:",
			 long_options, &option_index);
     
	/* Detect the end of the options. */
	if (c == -1)
	    break;
     
	switch (c)
	    {
	    case 'f':
		name = optarg;
		break;
	    case 'm':
		method = optarg;
		break;
	    case 't':
		threshold = atof(optarg);
		break;
	    case 'b':
                threshold_type = optarg;
		break;
            case 'c':
                threshold_count = atoi(optarg);
                break;
	    case 'o':
                output_file = optarg;
		break;
	    case '?':
		usage();
		break;
	    default:
		abort ();
	    }
    }

    if ( name == "" || output_file == "") {
	usage();
	return -1;
    }

    return 1; 
}

void print_coef(vector<Coefficient>& table)
{
    for (unsigned int i=0; i<table.size(); i++) {
	cout << table[i].x << ":" << table[i].y << " " << table[i].r << endl;
    }
}

void print_output(string allname, map<int, string>& backward_map, vector<Coefficient>& table, Profile& profile, double threshold)
{
    double coef = 10.0;  // Just some weird number...
    double pval = 0;
    unsigned int last_index = 0;

    ofstream out(output_file.c_str());

    out << "Annotation1" << "\t" << "Annotation2" << "\t" << "Correlation" << "\t" << "p-value" << "\t"
	 << "#Records(Anno1,Anno2)" << "\t" << "#Records(Anno1)" << "\t" << "#Records(Anno2)"
	 << "\t" << "#CoAnnos(Anno1)" << "\t" << "#CoAnnos(Anno2)" << endl;


    cout << "Pair size: " << table.size() << endl;
    

    for (unsigned int i=0; i<table.size(); i++) {
      if ( table[i].r < coef ) {
	last_index = i;
        coef = table[i].r;
      }
      pval = (double)last_index / table.size();

      if ( pval < threshold ) 
	out << backward_map[table[i].x] << "\t" << backward_map[table[i].y] << "\t" << table[i].r << "\t" << pval << "\t" << table[i].z << "\t" 
	     << profile.numSupRecs(table[i].x) << "\t" << profile.numSupRecs(table[i].y) << "\t" << profile.numCoAnnos(table[i].x) << "\t" << profile.numCoAnnos(table[i].y) << endl;
    }
    out.close();
}


int main(int argc, char **argv)
{
    vector<Coefficient> table;

    if (  parse_args(argc, argv) < 0 ) {
	exit(1);
    }

    Mapper mapper(name);
    Profile profile(mapper.get_mapped_file());

    if ( threshold_type == "by_item_count" ) {
      profile.filter_by_itemcount(threshold_count);
    }
    else if ( threshold_type == "by_cooccur_count" ) {
      profile.filter_by_cocount(threshold_count);
    }
    else {
      cout << "invalid filtering method: " << threshold_type << endl;
      exit(0);
    }


    if ( method == "pearson" ) {
	Metric::pearson(table, profile.prof());
    }
    else if ( method == "cosine" ) {
	Metric::cosine(table, profile.prof());
    }
    else if ( method == "spearman") {
	Metric::spearman(table, profile.prof());
    }
    else {
	cout << "invalid or unsupported method: " << method << endl;
	exit(0);
    }
    
    sort(table.begin(), table.end(), greaterThan);
    print_output(name+"_alloutput.txt", mapper.backmap(), table, profile, threshold);
}
