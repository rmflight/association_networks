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

#ifndef METRIC_H
#define METRIC_H

#include <map>
#include <vector>
#include <cmath>
#include "coefficient.h"

using namespace std;

class Metric 
{
public:
    static int max(int, int);
    
    static void quicksort(vector<int>& keys, vector<double>& vals, 
		   int left, int right);
    

    static void build_ordered_profile(map<int, map<int,double> >&ordered_profile,
				      map<int, double>& sd2,
				      map<int, double>& zrank,
				      double bar,
				      const map<int, map<int, double> >& profile);

    static int pearson(vector<Coefficient>& table, 
		       const map<int, map<int, double> >& profile);

    static int cosine(vector<Coefficient>& table, 
		      const map<int, map<int, double> >& profile);

    static int spearman(vector<Coefficient>& table, 
			const map<int, map<int, double> >& profile);

};

#endif
