#include "WorldTerrain.h"

using std::cout;
using std::endl;
using std::mt19937;
using std::uniform_int;
using std::uniform_real;
using std::variate_generator;

WorldTerrain::WorldTerrain()
	:
	const0(0),
	constStone(5),
	lowLand(anl::FBM, anl::GRADIENT, anl::QUINTIC),
	lowLandScaleOff(0.5, 0.0)
{
}

WorldTerrain::~WorldTerrain()
{
}

void WorldTerrain::Init(int seedVar)
{
	currSeed = seedVar;
}

void WorldTerrain::InputNewBoundary(unsigned short width, unsigned short depth, unsigned short height)
{
	currWidth = width;
	currDepth = depth;
	currHeight = height;
}

void WorldTerrain::GenerateRegularWorld()
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

	GenerateGround();
	//GenerateCaves();
	//worldTerrain.SetSourceModule(0, groundFinal);
	//worldTerrain.SetSourceModule(1, caveFinal);
}

void WorldTerrain::GenerateGround()
{
	groundGradient.setGradient(0, 0, 0, 8, 0, 0);

	worldTerrain.setLowSource(&const0);
	worldTerrain.setHighSource(&constStone);
	worldTerrain.setControlSource(&groundGradient);
	worldTerrain.setThreshold(0.005);
	return;

	lowLand.setNumOctaves(6);
	lowLand.setFrequency(2);

	lowLandScaleOff.setSource(&groundGradient);
}

/*
void WorldTerrain::GenerateCaves()
{
	caveShape1.SetSeed(currSeed + 111);
	caveShape1.SetOctaveCount(1);
	caveShape1.SetNoiseQuality(noise::QUALITY_BEST);
	caveShape1.SetFrequency(2);

	caveShapeSP1.SetSourceModule(0, caveShape1);
	caveShapeSP1.SetXScale(0.7);
	caveShapeSP1.SetYScale(0.7);
	caveShapeSP1.SetZScale(0.7);

	caveSelect1.SetSourceModule(0, const0);
	caveSelect1.SetSourceModule(1, constStone);
	caveSelect1.SetControlModule(caveShapeSP1);
	caveSelect1.SetBounds(-0.3, 256);

	caveShape2.SetSeed(currSeed + 222);
	caveShape2.SetOctaveCount(1);
	caveShape2.SetNoiseQuality(noise::QUALITY_BEST);
	caveShape2.SetFrequency(2);

	caveShapeSP2.SetSourceModule(0, caveShape2);
	caveShapeSP2.SetXScale(0.7);
	caveShapeSP2.SetYScale(0.7);
	caveShapeSP2.SetZScale(0.7);

	caveSelect2.SetSourceModule(0, const0);
	caveSelect2.SetSourceModule(1, constStone);
	caveSelect2.SetControlModule(caveShapeSP2);
	caveSelect2.SetBounds(-0.3, 256);

	caveMul.SetSourceModule(0, caveSelect1);
	caveMul.SetSourceModule(1, caveSelect2);
	
	caveTurb.SetSourceModule(0, caveMul);
	caveTurb.xDistortModule.SetSeed(currSeed + 333);
	caveTurb.yDistortModule.SetSeed(currSeed + 433);
	caveTurb.zDistortModule.SetSeed(currSeed + 533);
	caveTurb.xDistortModule.SetFrequency(2);
	caveTurb.zDistortModule.SetFrequency(2);
	caveTurb.xDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	caveTurb.yDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	caveTurb.zDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	caveTurb.SetXPower(0.25);
	caveTurb.SetYPower(0.25);
	caveTurb.SetZPower(0.25);

	caveInvert.SetSourceModule(0, caveTurb);
	caveInvert.SetScale(-1);
	caveInvert.SetBias(1);

	caveFinal.SetSourceModule(0, caveInvert);
}
*/