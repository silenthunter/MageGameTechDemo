#include "GameParser.h"

using std::ifstream;
using std::string;
using std::cerr;
using std::endl;

GameParser::GameParser(const string& gp_gameDataFP, const string& gp_chunkDataFP)
	: gameDataFP(gp_gameDataFP), 
	chunkDataFP(gp_chunkDataFP)
{

}

GameParser::~GameParser()
{

}

bool GameParser::ParseFile(const string& filepath, StrFloatMap& map)
{
	ifstream gameData((gameDataFP + filepath).c_str());
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
				cerr << "Error: GameParser.cpp/ParseFile(): end returned -1 on file: \"" << (gameDataFP + filepath) << "\"" << endl;
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
		cerr << "Unable to open filepath: " << (gameDataFP + filepath) << endl;
		return false;
	}
}

void GameParser::DataInput(StrFloatMap& map, const string& prefix, const string& data)
{
	map.insert(StrFloatMap::value_type(prefix, boost::lexical_cast<float>(data)));
}

bool GameParser::ParseIDFile(const std::string& filepath, StrU16Map& map)
{
	ifstream gameData((gameDataFP + filepath).c_str());
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
			if(end != string::npos)
			{
				string data = str.substr(0, end);
				string id = str.substr(end + 1);
				DataIDInput(map, data, id);
			}
			DataIDInput(map, str);
		}
		gameData.close();
		return true;
	}
	else
	{
		cerr << "Unable to open filepath: " << (gameDataFP + filepath) << endl;
		return false;
	}
}

void GameParser::DataIDInput(StrU16Map& map, const std::string& data)
{
	map.insert(StrU16Map::value_type(data, static_cast<unsigned short>(map.size() + 1)));
}

void GameParser::DataIDInput(StrU16Map& map, const std::string& data, const std::string& id)
{
	map.insert(StrU16Map::value_type(data, boost::lexical_cast<unsigned short>(id)));
}