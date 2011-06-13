#include "GameParser.h"
#include "GameGlobals.h"
#include <stdlib.h>
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <boost/lexical_cast.hpp>

using std::ifstream;
using std::string;
using std::cerr;
using std::endl;

bool ParseFile(const string& filepath, StrFloatMap& map)
{
	ifstream gameData(filepath.c_str());
	string str;

	if(gameData.is_open())
	{
		while(gameData.good())
		{
			getline(gameData, str);
			if(str.length() == 0 || str[0] == '[' || str[0] == '#') //Don't bother with [Category] lines or comments
			{
				continue;
			}
			size_t end = str.find('=');
			if(end == string::npos)
			{
				cerr << "Error: GameParser.cpp/ParseFile(): end returned -1 on file: \"" << filepath << "\"" << endl;
				gameData.close();
				return false;
			}
			string prefix = str.substr(0, end);
			string data = str.substr(end + 1);
			DataInput(map, prefix, data);
		}
		gameData.close();
		return true;
	}
	else
	{
		cerr << "Unable to open filepath: " << filepath << endl;
		return false;
	}
}

void DataInput(StrFloatMap& map, const string& prefix, const string& data)
{
	map.insert(StrFloatMap::value_type(prefix, boost::lexical_cast<float>(data)));
}