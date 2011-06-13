
#include "NoiseGeneration.h"
#include "noiseutils.h"
#include <Ogre.h>

#include "GameGlobals.h"
#include "GameParser.h"

#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/SimpleVolume.h"
#include <PolyVoxUtil/VolumeChangeTracker.h>
#include <PolyVoxCore/MeshDecimator.h>
#include <PolyVoxCore/SurfaceExtractor.h>

using namespace PolyVox;
using namespace Ogre;

void InitVoxels(SimpleVolume<MaterialDensityPair44>& volData, utils::NoiseMap& heightMap)
{
	for(int i = 0; i < heightMap.GetHeight(); i++)
	{
		for(int j = 0; j < heightMap.GetWidth(); j++)
		{
			int depth = heightMap.GetValue(j, i) * 64;
			if(depth >= 64) depth = 64;
			if(depth < 0) depth = 0;

			depth += 64;

			for(; depth >= 0; depth--)
			{
				PolyVox::MaterialDensityPair44 vox = volData.getVoxelAt(Vector3DInt32(j, depth, i));
				vox.setDensity(MaterialDensityPair44::getMaxDensity());
				volData.setVoxelAt(Vector3DInt32(j, depth, i), vox);
			}
			if(i % 32 == 0 && j % 32 == 0)
				printf("#");
		}
		if(i % 32 == 0)
			printf("\n");
	}
}

std::list<Ogre::Vector3> GetChunks(Ogre::Vector3& start, Ogre::Vector3& end)
{
	std::list<Ogre::Vector3> retn;

	int chunkSize = WorldDataMap["ChunkSize"];

	for(int i = start.x / chunkSize; i < end.x / chunkSize; i++)
	{
		for(int j = start.y / chunkSize; j < end.y / chunkSize; j++)
		{
			for(int k = start.z / chunkSize; k < end.z / chunkSize; k++)
			{
				retn.push_back(Vector3(i, j, k));
			}
		}
	}

	return retn;
}

int main(int argc, char* argv[])
{
	ParseFile("GameFiles/WorldData.ini", WorldDataMap);
	ParseFile("GameFiles/ItemData.ini", ItemDataMap);
	ParseFile("GameFiles/CharacterData.ini", CharacterDataMap);
	ParseFile("GameFiles/WorldGeneration.ini", WorldGenerationMap);
	ParseFile("GameFiles/WaterData.ini", WaterDataMap);

	utils::NoiseMap heightMap = GenerateRandomTerrainMap(-1, 8, 4);
	SimpleVolume<MaterialDensityPair44> volData(PolyVox::Region(Vector3DInt32(0,0,0), Vector3DInt32(heightMap.GetWidth(), 128, heightMap.GetHeight())));
	InitVoxels(volData, heightMap);

}