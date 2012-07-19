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

#include "profile.h"
#include <stdlib.h>

Profile::Profile()
{
}

Profile::Profile(string name)
{
    build_profile(name);
}

int Profile::build_profile(string name)
{
    string line;
    vector<int> items;
    
    input.open(name.c_str(), ifstream::in);
    
    if (!input.is_open() ) {
	cout << "Unable to open " << name << endl;
	return -1;
    }
    //    cout << "start profiling ...\n";
    
    while ( input.good() && !input.eof()) {
	int item;
	getline(input, line);
	
	if ( line.length() > 1 ) {
	    istringstream stream(line);
	    while ( stream >> item ) {
		item_count[item]++;
		items.push_back(item);
	    }

	    for (unsigned int i=0; i<items.size(); i++) {
	      profile[items[i]][items[i]]++;
	      for (unsigned int j=i+1; j<items.size(); j++) {
		profile[items[i]][items[j]]++;
		profile[items[j]][items[i]]++;
	      }
	    }
	    items.clear();
	}
    }
    
    input.close();

    //    cout << "done." <<  endl;
    
    return 0;
}


int Profile::read_profile(string name)
{
    string line;
    vector<int> items;
    int row = 1;
    
    input.open(name.c_str(), ifstream::in);
    
    if (!input.is_open() ) {
	cout << "Unable to open " << name << endl;
	return -1;
    }
    
    while ( input.good() && !input.eof()) {
	getline(input, line);
	int idx = 1;
        int item;
	
	if ( line.length() > 1 ) {
	    istringstream stream(line);
	    while ( stream >> item ) {
		if ( item > 0 ) {
		    profile[row][idx] = item;
		}
                idx++;
	    }
	    row++;
	}
    }
    
    input.close();

    //    cout << "done." <<  endl;
    
    return 0;
}

int Profile::filter_by_itemcount(int threshold)
{
    vector<int> list;
    map<int, int>::iterator iter;
    
    for (iter=item_count.begin(); iter!=item_count.end(); iter++) {
	if ( iter->second < threshold ) {
	    list.push_back(iter->first);
	    profile.erase(iter->first);
	}
    }

    adjust_profile(list);
    
    return 1;
}
 
int Profile::filter_by_cocount(int threshold)
{
  vector<int> list;
   map<int, map<int, double> >::iterator iter;
    
   for (iter=profile.begin(); iter != profile.end(); iter++) {
	map<int, double>::iterator citer;
	vector<int> elims;
	for (citer=iter->second.begin(); citer!=iter->second.end(); citer++) {
	  if ( citer->first != iter->first && citer->second < threshold ) {
	    elims.push_back(citer->first);
	  }
	}

	for (unsigned int i=0; i<elims.size(); i++) {
	  iter->second.erase(elims[i]);
	}

        // if it does not cooccur with anything else...
	if ( iter->second.size() == 1 ) {
	  list.push_back(iter->first);
	}
        elims.clear();
    }

   for (unsigned int i=0; i<list.size(); i++) {
     profile.erase(list[i]);
   }

   adjust_profile(list);

   return 1;
}

void Profile::adjust_profile(vector<int>& list)
{
  map<int, map<int, double> >::iterator iter;

  for(iter=profile.begin(); iter!=profile.end(); iter++) {
    for (unsigned int i=0; i<list.size(); i++) {
      map<int, double>::iterator it;
      if ( (it=iter->second.find(list[i])) != iter->second.end() ) {
	iter->second.erase(it);
      }
    }
  }
}

int Profile::numCoAnnos(int id)
{
  map<int, map<int, double> >::iterator iter = profile.find(id);

  if ( iter == profile.end() ) {
    return -1;
  }

  return (iter->second).size()-1;
}

int Profile::numCoRecs(int a, int b)
{
  double corecs = 0;

  map<int, map<int, double> >::iterator iter;

  if ( profile.find(a) == profile.end() || profile.find(b) == profile.end() ) 
	return 0;

  iter = profile.find(a);
  corecs = iter->second.find(b)->second;

  return (int) corecs;
}

ostream &operator<<(ostream &stream, Profile& prof)
{
  map<int, map<int, double> >::iterator iter;
  
  for (iter=prof.prof().begin(); iter!=prof.prof().end(); iter++) {
    stream << iter->first << " : ";
    map<int, double>::iterator iter2 = iter->second.begin();
    for(; iter2 != iter->second.end(); iter2++) {
      stream <<" "<< iter2->first << ":" << iter2->second;
    }
    stream<<endl;
    
  }
   
  return stream;
}


void Profile::print(ostream &stream, map<int,string>& back_map) 
{
  map<int, map<int, double> >::iterator iter;
  
  for (iter=profile.begin(); iter!=profile.end(); iter++) {
    
    stream << back_map[iter->first] << " : ";
    map<int, double>::iterator iter2 = iter->second.begin();
    for(; iter2 != iter->second.end(); iter2++) {
      stream <<" "<< back_map[iter2->first] << ":" << iter2->second;
    }
    stream<<endl;
  }
}




