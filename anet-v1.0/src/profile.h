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

#ifndef PROFILE_H
#define PROFILE_H
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <list>
#include <vector>
#include <cmath>

using namespace std;

class Profile
{
public:
  Profile();
  Profile(string name);
  int build_profile(string name);
  int read_profile(string name);
  int filter_by_itemcount(int threshold);
  int filter_by_cocount(int threshold);

  friend ostream &operator<<(ostream &stream, Profile& prof);
  
  map<int, map<int, double> >& prof() { return profile; };

  int numCoAnnos(int id);
  int numCoRecs(int a, int b);
  int numSupRecs(int id) { return item_count[id]; };

  void print(ostream&, map<int,string>&);

 private:
  void adjust_profile(vector<int>&);
  ifstream input;
  map<int,map<int,double> > profile;
  map<int,int> item_count;
};

#endif
