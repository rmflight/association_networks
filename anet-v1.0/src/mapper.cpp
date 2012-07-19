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

#include "mapper.h"

Mapper::Mapper()
{
}

Mapper::Mapper(string name)
{
    mapped_file = name + ".2num";
    map_file = name + "_map.txt";
    to_numeric(name);
    print_backward_map();
}

int Mapper::to_numeric(string name)
{
    string line;
    int index = 1;
    vector<int> items_line;

    input.open(name.c_str(), ifstream::in);
    
    if (!input.is_open() ) {
	cout << "Unable to open " << name << endl;
	return -1;
    }

    ofstream mappedfile(mapped_file.c_str());
    //    ofstream mapfile(map_file.c_str());    

    while ( input.good() && !input.eof()) {
	string item;
	getline(input, line);

	if ( line.length() > 1 ) {
	    istringstream stream(line);
	    items_line.clear();
	
	    while ( stream >> item && item != "" ) {
		if ( forward_map.find(item) == forward_map.end() ) {
		    forward_map[item] = index;
		    backward_map[index++] = item;
		}
	    
		items_line.push_back(forward_map[item]);
	    }

	    mappedfile << items_line[0];
	    for (unsigned int i=1; i<items_line.size(); i++) {
		mappedfile << "\t" << items_line[i];
	    }
	    mappedfile << endl;
	}
    }
    
    input.close();
    mappedfile.close();
    
    return 0;
}

void Mapper::print_backward_map()
{
  map<int, string>::iterator iter;
  ofstream mapfile(map_file.c_str());  
  for (iter= backward_map.begin(); iter!=backward_map.end(); iter++) {
    mapfile << iter->first << "\t" << iter->second << endl;
  }
  mapfile.close();
}

int Mapper::to_string(string name)
{
    return 0;
}
