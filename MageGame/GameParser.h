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

	//Parsing data files
	bool ParseFile(const std::string& filepath, StrFloatMap& map);
	bool ParseIDFile(const std::string& filepath, StrU16Map& map);
	void DataInput(StrFloatMap& map, const std::string& prefix, const std::string& data);
	void DataIDInput(StrU16Map& map, const std::string& data);
	void DataIDInput(StrU16Map& map, const std::string& data, const std::string& id);

	//Chunk saving/loading
	void SetPolyVolume(PolyVox::SimpleVolume<VoxelMat>* gp_polyVolume);
	bool PlayerMoveStore(int xDiff, int zDiff, int horizontalChunk, int verticalChunk, int direction);
	bool StoreChunk(PolyVox::Vector3DInt32 chunkNum, int xDiff, int zDiff);
	bool LoadChunk(PolyVox::Vector3DInt32 chunkNum, int xDiff, int zDiff);

private:
	int chunkSize;
	std::string gameDataFP;
	std::string chunkDataFP;
	PolyVox::SimpleVolume<VoxelMat>* polyVolume;
};