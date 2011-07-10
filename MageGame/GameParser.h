#pragma once

#include <string>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include "PolyVoxCore/SimpleVolume.h"

#include "GameGlobals.h"

class GameParser
{
public:
	GameParser(const std::string& gp_gameDataFP, const std::string& gp_chunkDataFP);
	~GameParser();
	void SetChunkSize(int gp_chunkSize);
	void SetVerticalChunk(int gp_verticalChunk);

	//Parsing data files
	bool ParseFile(const std::string& filepath, StrFloatMap& map);
	bool ParseIDFile(const std::string& filepath, StrU16Map& map);
	void DataInput(StrFloatMap& map, const std::string& prefix, const std::string& data);
	void DataIDInput(StrU16Map& map, const std::string& data);
	void DataIDInput(StrU16Map& map, const std::string& data, const std::string& id);

	//Chunk saving/loading
	void SetPolyVolume(PolyVox::SimpleVolume<VoxelMat>* gp_polyVolume);
	bool StoreChunks(int chunkNumX, int chunkNumZ, int xDiff, int zDiff);
	bool LoadChunks(int chunkNumX, int chunkNumZ, int xDiff, int zDiff);

private:
	int chunkSize;
	int verticalChunk;
	std::string gameDataFP;
	std::string chunkDataFP;
	PolyVox::SimpleVolume<VoxelMat>* polyVolume;
};