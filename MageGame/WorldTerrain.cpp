#include "WorldTerrain.h"

using std::cout;
using std::endl;
using std::mt19937;
using std::uniform_int;
using std::uniform_real;
using std::variate_generator;

WorldTerrain::WorldTerrain()
{
	constStone.SetConstValue(2);
	constDirt.SetConstValue(4);
	constGrass.SetConstValue(5);
	constSand.SetConstValue(7);
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

	//Basic ground
	groundBase.SetSeed(currSeed);
	groundBase.SetSourceModule(0, groundGradient);
	groundBase.SetPower(0.2);
	groundBase.yDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundBase.yDistortModule.SetOctaveCount(8);

	//Plain region
	groundPlain.SetSeed(currSeed + 22);
	groundPlain.SetSourceModule(0, groundGradient);
	groundPlain.SetPower(0.1);
	groundPlain.yDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundPlain.yDistortModule.SetOctaveCount(2);

	//Hilly region
	groundHill.SetSeed(currSeed + 11);
	groundHill.SetSourceModule(0, groundGradient);
	groundHill.SetPower(0.3);
	groundHill.SetFrequency(1.75);
	groundHill.yDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundHill.yDistortModule.SetOctaveCount(3);

	//Rolling hills
	groundRolling.SetSeed(currSeed + 33);
	groundRolling.SetSourceModule(0, groundGradient);
	groundRolling.SetPower(0.3);
	groundRolling.SetFrequency(1.75);
	groundRolling.yDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundRolling.yDistortModule.SetOctaveCount(1);

	//Small mountain ranges
	groundSmallMountain.SetSourceModule(0, groundGradient);
	groundSmallMountain.xDistortModule.SetSeed(currSeed + 44);
	groundSmallMountain.yDistortModule.SetSeed(currSeed + 55);
	groundSmallMountain.zDistortModule.SetSeed(currSeed + 66);
	groundSmallMountain.xDistortModule.SetFrequency(2);
	groundSmallMountain.yDistortModule.SetFrequency(3);
	groundSmallMountain.zDistortModule.SetFrequency(2);
	groundSmallMountain.xDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundSmallMountain.yDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundSmallMountain.zDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundSmallMountain.xDistortModule.SetOctaveCount(2);
	groundSmallMountain.yDistortModule.SetOctaveCount(2);
	groundSmallMountain.zDistortModule.SetOctaveCount(2);
	groundSmallMountain.SetXPower(0.3);
	groundSmallMountain.SetYPower(0.8);
	groundSmallMountain.SetZPower(0.3);
	groundSmallMountain.ySBModule.SetBias(-0.85);

	//Medium mountain ranges
	groundMediumMountain.SetSourceModule(0, groundGradient);
	groundMediumMountain.xDistortModule.SetSeed(currSeed + 77);
	groundMediumMountain.yDistortModule.SetSeed(currSeed + 88);
	groundMediumMountain.zDistortModule.SetSeed(currSeed + 99);
	groundMediumMountain.xDistortModule.SetFrequency(2);
	groundMediumMountain.yDistortModule.SetFrequency(3);
	groundMediumMountain.zDistortModule.SetFrequency(2);
	groundMediumMountain.xDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundMediumMountain.yDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundMediumMountain.zDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundMediumMountain.xDistortModule.SetOctaveCount(2);
	groundMediumMountain.yDistortModule.SetOctaveCount(2);
	groundMediumMountain.zDistortModule.SetOctaveCount(2);
	groundMediumMountain.SetXPower(0.3);
	groundMediumMountain.SetYPower(0.8);
	groundMediumMountain.SetZPower(0.3);
	groundMediumMountain.ySBModule.SetBias(-1.4);

	//Tall mountain ranges
	groundMountain.SetSourceModule(0, groundGradient);
	groundMountain.xDistortModule.SetSeed(currSeed + 110);
	groundMountain.yDistortModule.SetSeed(currSeed + 121);
	groundMountain.zDistortModule.SetSeed(currSeed + 132);
	groundMountain.xDistortModule.SetFrequency(2);
	groundMountain.yDistortModule.SetFrequency(3);
	groundMountain.zDistortModule.SetFrequency(2);
	groundMountain.xDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundMountain.yDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundMountain.zDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundMountain.xDistortModule.SetOctaveCount(2);
	groundMountain.yDistortModule.SetOctaveCount(2);
	groundMountain.zDistortModule.SetOctaveCount(2);
	groundMountain.SetXPower(0.3);
	groundMountain.SetYPower(0.8);
	groundMountain.SetZPower(0.3);
	groundMountain.ySBModule.SetBias(-2.25);

	//Small chasms
	groundSmallChasm.SetSourceModule(0, groundGradient);
	groundSmallChasm.xDistortModule.SetSeed(currSeed + 143);
	groundSmallChasm.yDistortModule.SetSeed(currSeed + 154);
	groundSmallChasm.zDistortModule.SetSeed(currSeed + 165);
	groundSmallChasm.xDistortModule.SetFrequency(2);
	groundSmallChasm.yDistortModule.SetFrequency(3);
	groundSmallChasm.zDistortModule.SetFrequency(2);
	groundSmallChasm.xDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundSmallChasm.yDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundSmallChasm.zDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundSmallChasm.xDistortModule.SetOctaveCount(2);
	groundSmallChasm.yDistortModule.SetOctaveCount(2);
	groundSmallChasm.zDistortModule.SetOctaveCount(2);
	groundSmallChasm.SetXPower(0.3);
	groundSmallChasm.SetYPower(0.8);
	groundSmallChasm.SetZPower(0.3);
	groundSmallChasm.ySBModule.SetBias(0.5);

	//Huge chasms
	groundChasm.SetSourceModule(0, groundGradient);
	groundChasm.xDistortModule.SetSeed(currSeed + 176);
	groundChasm.yDistortModule.SetSeed(currSeed + 187);
	groundChasm.zDistortModule.SetSeed(currSeed + 198);
	groundChasm.xDistortModule.SetFrequency(2);
	groundChasm.yDistortModule.SetFrequency(3);
	groundChasm.zDistortModule.SetFrequency(2);
	groundChasm.xDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundChasm.yDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundChasm.zDistortModule.SetNoiseQuality(noise::QUALITY_BEST);
	groundChasm.xDistortModule.SetOctaveCount(2);
	groundChasm.yDistortModule.SetOctaveCount(2);
	groundChasm.zDistortModule.SetOctaveCount(2);
	groundChasm.SetXPower(0.3);
	groundChasm.SetYPower(0.8);
	groundChasm.SetZPower(0.3);
	groundChasm.ySBModule.SetBias(1.25);

	//Selection begins

	//Medium mountain + tall mountain
	selPer1.SetSeed(currSeed + 209);
	selPer1.SetFrequency(0.5);
	selPer1.SetPersistence(0.25);

	selSP1.SetSourceModule(0, selPer1);
	selSP1.SetYScale(0);

	groundMediumTallMtnSelect.SetSourceModule(0, groundMountain);
	groundMediumTallMtnSelect.SetSourceModule(1, groundMediumMountain);
	groundMediumTallMtnSelect.SetControlModule(selSP1);
	groundMediumTallMtnSelect.SetBounds(-0.1, 256);
	groundMediumTallMtnSelect.SetEdgeFalloff(0.125);

	//Small mountain + medium mountain + tall mountain
	selPer2.SetSeed(currSeed + 231);
	selPer2.SetFrequency(0.5);
	selPer2.SetPersistence(0.25);

	selSP2.SetSourceModule(0, selPer2);
	selSP2.SetYScale(0);

	groundMountainSelect.SetSourceModule(0, groundMediumTallMtnSelect);
	groundMountainSelect.SetSourceModule(1, groundSmallMountain);
	groundMountainSelect.SetControlModule(selSP2);
	groundMountainSelect.SetBounds(-0.1, 256);
	groundMountainSelect.SetEdgeFalloff(0.125);

	//Small chasm + huge chasm
	selPer3.SetSeed(currSeed + 242);
	selPer3.SetFrequency(0.5);
	selPer3.SetPersistence(0.25);

	selSP3.SetSourceModule(0, selPer3);
	selSP3.SetYScale(0);

	groundChasmSelect.SetSourceModule(0, groundChasm);
	groundChasmSelect.SetSourceModule(1, groundSmallChasm);
	groundChasmSelect.SetControlModule(selSP3);
	groundChasmSelect.SetBounds(-0.2, 256);
	groundChasmSelect.SetEdgeFalloff(0.125);

	//Hill + rolling hills
	selPer4.SetSeed(currSeed + 253);
	selPer4.SetFrequency(0.5);
	selPer4.SetPersistence(0.25);

	selSP4.SetSourceModule(0, selPer4);
	selSP4.SetYScale(0);

	groundHillRollingSelect.SetSourceModule(0, groundRolling);
	groundHillRollingSelect.SetSourceModule(1, groundHill);
	groundHillRollingSelect.SetControlModule(selSP4);
	groundHillRollingSelect.SetBounds(-0.2, 256);
	groundHillRollingSelect.SetEdgeFalloff(0.125);

	//Plain + hill + rolling hills
	selPer5.SetSeed(currSeed + 264);
	selPer5.SetFrequency(0.5);
	selPer5.SetPersistence(0.25);

	selSP5.SetSourceModule(0, selPer5);
	selSP5.SetYScale(0);

	groundPlainHillRollingSelect.SetSourceModule(0, groundHillRollingSelect);
	groundPlainHillRollingSelect.SetSourceModule(1, groundPlain);
	groundPlainHillRollingSelect.SetControlModule(selSP5);
	groundPlainHillRollingSelect.SetBounds(0, 256);
	groundPlainHillRollingSelect.SetEdgeFalloff(0.125);

	//Basic + plain + hill + sand dunes
	selPer6.SetSeed(currSeed + 275);
	selPer6.SetFrequency(0.5);
	selPer6.SetPersistence(0.25);

	selSP6.SetSourceModule(0, selPer6);
	selSP6.SetYScale(0);

	groundBaseSelect.SetSourceModule(0, groundPlainHillRollingSelect);
	groundBaseSelect.SetSourceModule(1, groundBase);
	groundBaseSelect.SetControlModule(selSP6);
	groundBaseSelect.SetBounds(0, 256);
	groundBaseSelect.SetEdgeFalloff(0.125);

	//Base + mountains
	selPer7.SetSeed(currSeed + 286);
	selPer7.SetFrequency(0.5);
	selPer7.SetPersistence(0.25);

	selSP7.SetSourceModule(0, selPer7);
	selSP7.SetYScale(0);

	groundBaseMtnSelect.SetSourceModule(0, groundMountainSelect);
	groundBaseMtnSelect.SetSourceModule(1, groundBaseSelect);
	groundBaseMtnSelect.SetControlModule(selSP7);
	groundBaseMtnSelect.SetBounds(-0.1, 256);
	groundBaseMtnSelect.SetEdgeFalloff(0.125);

	//Final Terrain
	selPer8.SetSeed(currSeed + 297);
	selPer8.SetFrequency(0.5);
	selPer8.SetPersistence(0.25);

	selSP8.SetSourceModule(0, selPer8);
	selSP8.SetYScale(0);

	groundFinalSelect.SetSourceModule(0, groundChasmSelect);
	groundFinalSelect.SetSourceModule(1, groundBaseMtnSelect);
	groundFinalSelect.SetControlModule(selSP8);
	groundFinalSelect.SetBounds(-0.1, 256);
	groundFinalSelect.SetEdgeFalloff(0.125);

	//Final Selection
	groundMatSelect.SetSourceModule(0, const0);
	groundMatSelect.SetSourceModule(1, constGrass);
	groundMatSelect.SetControlModule(groundFinalSelect);
	groundMatSelect.SetBounds(0, 256);

	//Final Result
	worldTerrain.SetSourceModule(0, groundMatSelect);
}

void WorldTerrain::GenerateCaves()
{
	caveShape1.SetSeed(currSeed + 111);
	caveShape1.SetOctaveCount(1);
	caveShape1.SetNoiseQuality(noise::QUALITY_BEST);
	caveShape1.SetFrequency(2);

	caveSelect1.SetSourceModule(0, const0);
	caveSelect1.SetSourceModule(1, constDirt);
	caveSelect1.SetControlModule(caveShape1);
	caveSelect1.SetBounds(0.1, 256);

	caveShape2.SetSeed(currSeed + 222);
	caveShape2.SetOctaveCount(1);
	caveShape2.SetNoiseQuality(noise::QUALITY_BEST);
	caveShape2.SetFrequency(2);

	caveSelect2.SetSourceModule(0, const0);
	caveSelect2.SetSourceModule(1, constDirt);
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