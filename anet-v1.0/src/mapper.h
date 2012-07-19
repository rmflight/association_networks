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

#ifndef MAPPER_H
#define MAPPER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>

using namespace std;

class Mapper
{
    public:
    Mapper();
    Mapper(string name);
    
    int to_numeric(string name);
    int to_string(string name);

    string get_mapped_file() {
      return mapped_file;
    };

    map<int, string>& backmap() { return backward_map; };

    void print_backward_map();
 private:
  ifstream input;
  string mapped_file;
  string map_file;
  
  map<string, int> forward_map;
  map<int, string> backward_map;
};

#endif
