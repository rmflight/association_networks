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

#include "metric.h"
#include <iostream>
#include <stdlib.h>

int
Metric::max(int a, int b)
{
    if ( a > b ) return a;
    
    return b;
}

void 
Metric::quicksort(vector<int>& keys, vector<double>& vals, 
		  int left, int right)
{
  int i = left, j=right;
  int tmp;
  
  int pivot = vals[(left + right) / 2];
  
  while ( i <= j ) 
    {
      
      while ( vals[i] > pivot ) i++;
      while ( vals[j] < pivot ) j--;
      
      if ( i <= j ) 
	{
	  tmp = vals[i];
	  vals[i] = vals[j];
	  vals[j] = tmp;
	  
	  tmp = keys[i];
	  keys[i] = keys[j];
	  keys[j] = tmp;

	  i++;
	  j--;
	}
    }
  
	  
  if (left < j ) 
    quicksort(keys, vals, left, j);
  if ( i < right )
    quicksort(keys, vals, i, right);
}



/**
 *
 *
 *
 */
int Metric::cosine(vector<Coefficient>& table,
		   const map<int, map<int, double> >& profile)
{
    map<int, map<int, double> >::const_iterator iter;
    double sig_x_2 = 0;
    double sig_y_2 = 0;
    double prod_xy = 0;
    int cooc = 0;
    
    for (iter=profile.begin(); iter!=profile.end(); iter++) {
        map<int, double>::const_iterator xiter;
	sig_x_2 = 0;

	for (xiter=iter->second.begin(); xiter!=iter->second.end(); xiter++) {
	    sig_x_2 += xiter->second*xiter->second;
	}

	map<int, map<int, double> >::const_iterator jter = iter;

	for (++jter; jter != profile.end(); jter++) {
	    map<int, double>::const_iterator yiter;

	    sig_y_2 = 0;
	    prod_xy = 0;
	    for (yiter=jter->second.begin(); yiter!=jter->second.end(); yiter++) {
		sig_y_2 += yiter->second*yiter->second;
		
		if ( iter->second.find(yiter->first) != iter->second.end() ) {
		    prod_xy += yiter->second * iter->second.find(yiter->first)->second;
		}
	    }

	    double cor = prod_xy / (sqrt(sig_x_2) * sqrt(sig_y_2));
	    cooc = 0;

	    map<int, double>::const_iterator it;
	    if ( (it=iter->second.find(jter->first)) != iter->second.end() ) {
		cooc = (int) it->second;
	    }
	
	    Coefficient coef(iter->first,jter->first, cooc, cor);
	    table.push_back(coef);

        }
    }
    
    return 0;
}


/**
 *
 * 
 */
int Metric::pearson(vector<Coefficient>& table,
		    const map<int, map<int, double> >& profile)
{
    double sig_y = 0;
    double sig_y_2 = 0;
    double sig_xy = 0;
    double sig_x = 0;
    double sig_x_2 = 0;
    int N = profile.size();
    int cooc = 0;

    map<int, map<int, double> >::const_iterator iter;
    for (iter=profile.begin(); iter != profile.end(); iter++) {
	map<int, double>::const_iterator xiter;
	sig_x = sig_x_2 = 0;

	for (xiter=iter->second.begin(); xiter!=iter->second.end(); xiter++) {
	    sig_x += xiter->second;
	    sig_x_2 += xiter->second*xiter->second;
	}

	map<int, map<int, double> >::const_iterator jter = iter;
	for (++jter; jter != profile.end(); jter++) {
	    map<int, double>::const_iterator yiter;
	    sig_y = sig_y_2 = sig_xy = 0;

	    for (yiter=jter->second.begin(); yiter!=jter->second.end(); yiter++) {
		sig_y += yiter->second;
		sig_y_2 += yiter->second*yiter->second;
		if ( iter->second.find(yiter->first) != iter->second.end() ) {
		    sig_xy += yiter->second * iter->second.find(yiter->first)->second;
		}
	    }

	    double cor = sig_xy - sig_x * sig_y / N;
	    cor /= sqrt( (sig_x_2 - sig_x*sig_x/N) * (sig_y_2 - sig_y*sig_y/N) );
	    
	    cooc = 0;
	    map<int, double>::const_iterator it;
	    if ( (it=iter->second.find(jter->first)) != iter->second.end() ) {
		cooc = (int) it->second;
            }
	
	    Coefficient coef(iter->first,jter->first, cooc, cor);
            table.push_back(coef);
	}
    }

    return 0;
}


void 
Metric::build_ordered_profile(map<int, map<int,double> >&ordered_profile,
			      map<int, double>& sd2, 
			      map<int, double>& zrank,
			      double bar,
			      const map<int, map<int, double> >& profile)
{
    unsigned int i,j,k,t;
    map<int, map<int, double> >::const_iterator iter;
    
    for (iter=profile.begin(); iter!=profile.end(); iter++) {
      vector<int> keys;
      vector<double> vals;
      
      map<int,double>::const_iterator it;
      
      for (it=iter->second.begin(); it !=iter->second.end(); it++) {
	  keys.push_back(it->first);
	  vals.push_back(it->second);
      }

      quicksort(keys, vals, 0, keys.size()-1);

      i = 0;
      
      while ( i < keys.size() ) 
	{
	  if ( vals[i] != vals[i+1] ) {
	      ordered_profile[iter->first][keys[i]] = i+1;
	      i++;
	  }
	  else {
	    for (j=i+1; j<keys.size()&& vals[j]==vals[i]; j++);
	      double rank = 0.5 * (i+ 1 + j);
	      for (k=i; k<j;k++) 
		{
		  ordered_profile[iter->first][keys[k]] = rank;
		}
	      
	      i=j;
	  }
      }
      
      if (i==keys.size()-1) 
	{
	  ordered_profile[iter->first][keys[i]] = keys.size();
	}
      

      if ( keys.size() < profile.size() ) {
        t = profile.size() - keys.size();
	zrank[iter->first] = 0.5 * (profile.size() + keys.size() );
      }

      for (it=iter->second.begin(); it != iter->second.end(); it++) 
	{
	  sd2[iter->first] += (it->second - bar) * (it->second - bar);
	}
      
      sd2[iter->first] += t * (zrank[iter->first] - bar) * (zrank[iter->first] - bar);
    }
}

int Metric::spearman(vector<Coefficient>& table,
		     const map<int, map<int, double> >& profile)
{
    int N = profile.size();
    map<int, map<int, double> > ordered_profile;
    map<int, double> sd2;
    map<int, double> zrank;
    double abar = (N+1)/2.0;
    map<int, map<int, double> >::const_iterator iter;
    
    build_ordered_profile(ordered_profile, sd2, zrank, abar,  profile);

    for (iter=ordered_profile.begin(); iter!=ordered_profile.end(); iter++) {
	map<int, map<int, double> >::const_iterator jter = iter;
	for (++jter; jter!=ordered_profile.end(); jter++) {

	  double numer = 0.0;
	  int ij = 0;
	  
	  map<int, double>::const_iterator it=iter->second.begin();
	    for (; it!=iter->second.end(); it++) {
		map<int, double>::const_iterator jt;
		if ( (jt=jter->second.find(it->first)) != jter->second.end() ) {
		  numer += (it->second - abar) * (jt->second - abar);
		  ij++;
		}
		else {
		  numer += (it->second - abar) * (zrank[jter->first]-abar);
		}
	    }

	    map<int, double>::const_iterator jt=jter->second.begin();
	    for (; jt!=jter->second.end(); jt++) {
		if ( (it=iter->second.find(jt->first)) == iter->second.end() ) {
		  numer += (zrank[iter->first] - abar ) * (jt->second - abar);
		}
	    }

	    int cooc = 0;

	    double cor = numer /sqrt(sd2[iter->first]*sd2[jter->first]);

	    if ( (it=iter->second.find(jter->first)) != iter->second.end() ) {
		cooc = (int) it->second;
	    }

	    Coefficient coef(iter->first, jter->first, cooc, cor);
            table.push_back(coef);
	}
    }

    return 0;
}
