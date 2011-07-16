#include "WorldTerrain.h"

using std::cout;
using std::endl;
using std::mt19937;
using std::uniform_int;
using std::uniform_real;
using std::variate_generator;

WorldTerrain::WorldTerrain()
{
	constMat.SetConstValue(1); //Start with stone
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
	groundGradient.SetGradient(0, 0, 0, 8, 0, 0);

	groundBase.SetSeed(currSeed);
	groundBase.SetSourceModule(0, groundGradient);
	groundBase.SetPower(0.2);
	groundBase.yDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundBase.yDistortModule.SetOctaveCount(8);

	groundHill.SetSeed(currSeed + 11);
	groundHill.SetSourceModule(0, groundGradient);
	groundHill.SetPower(0.3);
	groundHill.SetFrequency(1.75);
	groundHill.yDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundHill.yDistortModule.SetOctaveCount(3);

	groundPlain.SetSeed(currSeed + 22);
	groundPlain.SetSourceModule(0, groundGradient);
	groundPlain.SetPower(0.1);
	groundPlain.yDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundPlain.yDistortModule.SetOctaveCount(2);

	groundRolling.SetSeed(currSeed + 33);
	groundRolling.SetSourceModule(0, groundGradient);
	groundRolling.SetPower(0.3);
	groundRolling.SetFrequency(1.75);
	groundRolling.yDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundRolling.yDistortModule.SetOctaveCount(1);

	groundMountain.SetSourceModule(0, groundGradient);
	groundMountain.xDistortModule.SetSeed(currSeed + 44);
	groundMountain.yDistortModule.SetSeed(currSeed + 55);
	groundMountain.zDistortModule.SetSeed(currSeed + 66);
	groundMountain.xDistortModule.SetFrequency(3);
	groundMountain.yDistortModule.SetFrequency(3);
	groundMountain.zDistortModule.SetFrequency(3);
	groundMountain.xDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundMountain.yDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundMountain.zDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundMountain.xDistortModule.SetOctaveCount(1);
	groundMountain.yDistortModule.SetOctaveCount(1);
	groundMountain.zDistortModule.SetOctaveCount(1);
	groundMountain.SetXPower(0.6);
	groundMountain.SetYPower(0.8);
	groundMountain.SetZPower(0.6);

	selectPerlin.SetSeed(currSeed + 77);
	selectPerlin.SetFrequency(0.5);
	selectPerlin.SetPersistence(0.25);

	selectSP.SetSourceModule(0, selectPerlin);
	selectSP.SetYScale(0);
	
	groundBaseSelect.SetSourceModule(0, const0);
	groundBaseSelect.SetSourceModule(1, constMat);
	groundBaseSelect.SetControlModule(groundBase);
	groundBaseSelect.SetBounds(-0.015, 256);

	groundHillSelect.SetSourceModule(0, const0);
	groundHillSelect.SetSourceModule(1, constMat);
	groundHillSelect.SetControlModule(groundHill);
	groundHillSelect.SetBounds(0, 256);

	groundPlainSelect.SetSourceModule(0, const0);
	groundPlainSelect.SetSourceModule(1, constMat);
	groundPlainSelect.SetControlModule(groundPlain);
	groundPlainSelect.SetBounds(0, 256);

	groundRollingSelect.SetSourceModule(0, const0);
	groundRollingSelect.SetSourceModule(1, constMat);
	groundRollingSelect.SetControlModule(groundRolling);
	groundRollingSelect.SetBounds(0, 256);

	groundMountainSelect.SetSourceModule(0, const0);
	groundMountainSelect.SetSourceModule(1, constMat);
	groundMountainSelect.SetControlModule(groundMountain);
	groundMountainSelect.SetBounds(-0.2, 256);

	groundSelect1.SetSourceModule(0, groundMountainSelect);
	groundSelect1.SetSourceModule(1, groundRollingSelect);
	groundSelect1.SetControlModule(selectSP);
	groundSelect1.SetBounds(0, 256);

	selectPerlin.SetSeed(currSeed + 88);

	groundSelect2.SetSourceModule(0, groundHillSelect);
	groundSelect2.SetSourceModule(1, groundPlainSelect);
	groundSelect2.SetControlModule(selectSP);
	groundSelect2.SetBounds(0, 256);

	selectPerlin.SetSeed(currSeed + 99);

	groundSelect3.SetSourceModule(0, groundBaseSelect);
	groundSelect3.SetSourceModule(1, groundSelect2);
	groundSelect3.SetControlModule(selectSP);
	groundSelect3.SetBounds(0, 256);

	selectPerlin.SetSeed(currSeed + 110);

	groundFinalSelect.SetSourceModule(0, groundSelect3);
	groundFinalSelect.SetSourceModule(1, groundSelect1);
	groundFinalSelect.SetControlModule(selectSP);
	groundFinalSelect.SetBounds(0.1, 256);

	worldTerrain.SetSourceModule(0, groundFinalSelect);
}

void WorldTerrain::GenerateCaves()
{
	caveShape1.SetSeed(currSeed + 111);
	caveShape1.SetOctaveCount(1);
	caveShape1.SetNoiseQuality(noise::QUALITY_BEST);
	caveShape1.SetFrequency(2);

	caveSelect1.SetSourceModule(0, const0);
	caveSelect1.SetSourceModule(1, constMat);
	caveSelect1.SetControlModule(caveShape1);
	caveSelect1.SetBounds(0.1, 256);

	caveShape2.SetSeed(currSeed + 222);
	caveShape2.SetOctaveCount(1);
	caveShape2.SetNoiseQuality(noise::QUALITY_BEST);
	caveShape2.SetFrequency(2);

	caveSelect2.SetSourceModule(0, const0);
	caveSelect2.SetSourceModule(1, constMat);
	caveSelect2.SetControlModule(caveShape2);
	caveSelect2.SetBounds(0.1, 256);

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

	worldTerrain.SetSourceModule(0, caveSelect1);
}