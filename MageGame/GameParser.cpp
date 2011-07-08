#include "GameParser.h"

using std::ifstream;
using std::ofstream;
using std::string;
using std::cerr;
using std::endl;
using PolyVox::Vector3DInt32;

GameParser::GameParser(const string& gp_gameDataFP, const string& gp_chunkDataFP)
	: gameDataFP(gp_gameDataFP), 
	chunkDataFP(gp_chunkDataFP)
{

}

GameParser::~GameParser()
{

}

void GameParser::SetChunkSize(int gp_chunkSize)
{
	chunkSize = gp_chunkSize;
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

void GameParser::SetPolyVolume(PolyVox::SimpleVolume<VoxelMat>* gp_polyVolume)
{
	polyVolume = gp_polyVolume;
}

bool GameParser::PlayerMoveStore(int xDiff, int zDiff, int horizontalChunk, int verticalChunk, int direction)
{
	switch(direction)
	{
	case 0: //North
		{
		int lastChunk = horizontalChunk - 1;
		for(int i = 0; i < horizontalChunk; ++i)
		{
			for(int j = 0; j < verticalChunk; ++j)
			{
				StoreChunk(Vector3DInt32(i, j, lastChunk), xDiff, zDiff);
			}
		}
		break;
		}
	case 1: //East
		{
		int lastChunk = horizontalChunk - 1;
		for(int i = 0; i < horizontalChunk; ++i)
		{
			for(int j = 0; j < verticalChunk; ++j)
			{
				StoreChunk(Vector3DInt32(lastChunk, j, i), xDiff, zDiff);
			}
		}
		break;
		}
	case 2: //South
		{
		for(int i = 0; i < horizontalChunk; ++i)
		{
			for(int j = 0; j < verticalChunk; ++j)
			{
				StoreChunk(Vector3DInt32(i, j, 0), xDiff, zDiff);
			}
		}
		break;
		}
	case 3: //West
		{
		for(int i = 0; i < horizontalChunk; ++i)
		{
			for(int j = 0; j < verticalChunk; ++j)
			{
				StoreChunk(Vector3DInt32(0, j, i), xDiff, zDiff);
			}
		}
		break;
		}
	default:
		{
		cerr << "Direction: " << direction << " is unknown." << endl;
		return false;
		}
	}
	return true;
}

bool GameParser::StoreChunk(PolyVox::Vector3DInt32 chunkNum, int xDiff, int zDiff)
{
	unsigned int iChunkStart = chunkNum.getX() * chunkSize;
	unsigned int jChunkStart = chunkNum.getY() * chunkSize;
	unsigned int kChunkStart = chunkNum.getZ() * chunkSize;
	unsigned short chunkAddition = chunkSize - 1;

	unsigned int iChunkEnd = iChunkStart + chunkAddition;
	unsigned int jChunkEnd = jChunkStart + chunkAddition;
	unsigned int kChunkEnd = kChunkStart + chunkAddition;

	char chunkInfo[50];
	sprintf(chunkInfo, "%d_%d_%d", chunkNum.getX() + xDiff, chunkNum.getY(), chunkNum.getZ() + zDiff);

	string filepath = chunkDataFP;
	filepath += chunkInfo;

	ofstream chunkFile(filepath);
	if(!chunkFile.is_open())
	{
		cerr << "Unable to open filepath to save chunk: " << filepath << endl;
		return false;
	}

	for(int i = iChunkStart; i < iChunkEnd; ++i)
	{
		for(int j = jChunkStart; j < jChunkEnd; ++j)
		{
			for(int k = kChunkStart; k < kChunkEnd; ++k)
			{
				VoxelMat vox = polyVolume->getVoxelAt(Vector3DInt32(i, j, k));
				chunkFile << vox.getMaterial() << " ";
			}
		}
	}

	chunkFile.close();
	return true;
}

bool GameParser::LoadChunk(PolyVox::Vector3DInt32 chunkNum, int xDiff, int zDiff)
{
	char chunkInfo[50];
	sprintf(chunkInfo, "%d_%d_%d", chunkNum.getX() + xDiff, chunkNum.getY(), chunkNum.getZ() + zDiff);

	string filepath = chunkDataFP;
	filepath += chunkInfo;

	ifstream chunkFile(filepath);
	if(!chunkFile.is_open()) return false;

	unsigned int iChunkStart = chunkNum.getX() * chunkSize;
	unsigned int jChunkStart = chunkNum.getY() * chunkSize;
	unsigned int kChunkStart = chunkNum.getZ() * chunkSize;
	unsigned short chunkAddition = chunkSize - 1;

	unsigned int iChunkEnd = iChunkStart + chunkAddition;
	unsigned int jChunkEnd = jChunkStart + chunkAddition;
	unsigned int kChunkEnd = kChunkStart + chunkAddition;

	for(int i = iChunkStart; i < iChunkEnd; ++i)
	{
		for(int j = jChunkStart; j < jChunkEnd; ++j)
		{
			for(int k = kChunkStart; k < kChunkEnd; ++k)
			{
				VoxelMat vox = polyVolume->getVoxelAt(Vector3DInt32(i, j, k));
				int currMat;
				chunkFile >> currMat;
				vox.setMaterial(currMat);
			}
		}
	}

	chunkFile.close();
	return true;
}