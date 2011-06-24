#include "WorldTerrain.h"

using std::cout;
using std::endl;
using std::mt19937;
using std::uniform_int;
using std::uniform_real;
using std::variate_generator;

WorldTerrain::WorldTerrain()
{
	chunkSize = WorldDataMap["ChunkSize"];
}

WorldTerrain::~WorldTerrain()
{
}

void WorldTerrain::Init(int seedVar)
{
	currSeed = seedVar;
}

void WorldTerrain::Generate()
{
	/*
	mt19937 randGen((time_t) seedVar);

	//Mountain Terrain
	module::RidgedMulti mountainTerrain;
	mountainTerrain.SetSeed(seedVar);

	mountainTerrain.SetOctaveCount(RandomWorldGenerationMap["MountainOctave"]);
	//////////

	//Really flat terrain
	module::Billow baseFlatTerrain;
	baseFlatTerrain.SetSeed(seedVar);

	baseFlatTerrain.SetOctaveCount(RandomWorldGenerationMap["BaseFlatOctave"]);

	uniform_real<> baseFlatFrequencyRange(RandomWorldGenerationMap["BaseFlatFrequencyMin"], RandomWorldGenerationMap["BaseFlatFrequencyMax"]);
	baseFlatTerrain.SetFrequency(baseFlatFrequencyRange(randGen)); //Higher <value> means more but smaller lumps
	//////////

	//Modifying the flat terrain
	module::ScaleBias flatTerrain;
	flatTerrain.SetSourceModule(0, baseFlatTerrain);

	uniform_real<> flatScaleRange(RandomWorldGenerationMap["FlatScaleMin"], RandomWorldGenerationMap["FlatScaleMax"]);
	flatTerrain.SetScale(flatScaleRange(randGen)); //Shorten height by 1/<value>

	uniform_real<> flatBiasRange(RandomWorldGenerationMap["FlatBiasMin"], RandomWorldGenerationMap["FlatBiasMax"]);
	flatTerrain.SetBias(flatBiasRange(randGen)); //Translate height by <value>

	//////////

	//Perlin
	module::Perlin terrainType;
	terrainType.SetSeed(seedVar);

	terrainType.SetOctaveCount(RandomWorldGenerationMap["PerlinOctave"]); //Higher <value> increases detail

	//Low frequency and persistence value will create large areas of similar terrain in the final height map
	uniform_real<> perlinFrequencyRange(RandomWorldGenerationMap["PerlinFrequencyMin"], RandomWorldGenerationMap["PerlinFrequencyMax"]);
	terrainType.SetFrequency(perlinFrequencyRange(randGen));

	uniform_real<> perlinPersistenceRange(RandomWorldGenerationMap["PerlinPersistenceMin"], RandomWorldGenerationMap["PerlinPersistenceMax"]);
	terrainType.SetPersistence(perlinPersistenceRange(randGen));

	//Combining flat and mountain terrain, then using perlin as the controller
	module::Select terrainSelector;
	terrainSelector.SetSourceModule(0, flatTerrain);
	terrainSelector.SetSourceModule(1, mountainTerrain);
	terrainSelector.SetControlModule(terrainType);
	terrainSelector.SetBounds(0.0, 1000.0);

	uniform_real<> terrainEdgeFallOffRange(RandomWorldGenerationMap["TerrainEdgeFallOffMin"], RandomWorldGenerationMap["TerrainEdgeFallOffMax"]);
	terrainSelector.SetEdgeFalloff(terrainEdgeFallOffRange(randGen)); //Higher <value> means more smoothing

	//Setting it all as the final terrain
	module::Turbulence finalTerrain;
	finalTerrain.SetSeed(seedVar);

	finalTerrain.SetSourceModule(0, terrainSelector);

	uniform_real<> terrainFrequencyRange(RandomWorldGenerationMap["TerrainFrequencyMin"], RandomWorldGenerationMap["TerrainFrequencyMax"]);
	finalTerrain.SetFrequency(terrainFrequencyRange(randGen)); //<value> determines how often the coordinates of the input value changes

	uniform_real<> terrainPowerRange(RandomWorldGenerationMap["TerrainPowerMin"], RandomWorldGenerationMap["TerrainPowerMax"]);
	finalTerrain.SetPower(terrainPowerRange(randGen)); //<value> determines the magnitude of these changes
	*/

	//worldTerrain.SetSeed(currSeed);
	//worldTerrain.SetOctaveCount(WorldGenerationMap["PerlinOctave"]);

	//Const modules
	const1.SetConstValue(1);

	constNeg1.SetConstValue(-1);

	//Base Ground
	groundGradiant.SetGradient(0, 0, 0, 1, 0, 0);

	groundTurb.SetSeed(currSeed);
	groundTurb.SetSourceModule(0, groundGradiant);
	groundTurb.SetPower(0.3);
	groundTurb.SetFrequency(1.75);
	groundTurb.yDistortModule.SetOctaveCount(10);
	groundTurb.yDistortModule.SetNoiseQuality(noise::QUALITY_BEST);

	//worldTerrain.SetSourceModule(0, groundTurb);
	worldTerrain.SetSourceModule(0, constNeg1);
	worldTerrain.SetSourceModule(1, const1);
	worldTerrain.SetControlModule(groundTurb);
	worldTerrain.SetBounds(0.2, 1000);

	/*
	//Caves
	caveShape1.SetSeed(currSeed);
	caveShape1.SetOctaveCount(10);
	caveShape1.SetFrequency(2);

	caveShape2.SetSeed(currSeed + 666);
	caveShape2.SetOctaveCount(10);
	caveShape2.SetFrequency(2);

	caveMul.SetSourceModule(0, caveShape1);
	caveMul.SetSourceModule(1, caveShape2);
	
	caveTurb.SetSourceModule(0, caveMul);
	caveTurb.SetSeed(currSeed + 999);
	caveTurb.SetFrequency(3);
	caveTurb.SetPower(0.25);

	caveBias.SetSourceModule(0, caveTurb);
	caveBias.SetBias(-0.3);

	caveInvert.SetSourceModule(0, caveBias);

	//worldTerrain.SetSourceModule(0, groundBase);
	//worldTerrain.SetSourceModule(1, caveInvert);
	*/
}

/*
void GrabChunk(int xMin, int xMax, int zMin, int zMax)
{
	heightMapBuilder.SetSourceModule(worldTerrain);
	heightMapBuilder.SetDestNoiseMap(heightMap);

	int destSizeX = (xMax - xMin) * ((int) WorldDataMap["ChunkSize"]);
	int destSizeZ = (zMax - zMin) * ((int) WorldDataMap["ChunkSize"]);

	heightMapBuilder.SetDestSize(destSizeX, destSizeZ);
	heightMapBuilder.SetBounds(xMin, xMax, zMin, zMax);
	heightMapBuilder.Build();
}
*/

void WorldTerrain::InputNewBoundary(int width, int height, int depth)
{
	currWidth = width;
	currHeight = height;
	currDepth = depth;
}

double WorldTerrain::ReturnValue(int x, int y, int z)
{
	double nx = (double)x / (double)currWidth;
	double ny = (double)y / (double)currHeight;
	double nz = (double)z / (double)currDepth;

	return worldTerrain.GetValue(nx, ny, nz);
}