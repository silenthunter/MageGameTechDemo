#pragma once
#include <noise/noise.h>
#include <time.h>
#include <iostream>
#include <random>

#include "GameGlobals.h"
#include "Gradient.h"
#include "EnhancedTurbulence.h"
#include "YTurbulence.h"
#include "HillTurbulence.h"

class WorldTerrain
{
public:
	//Functions
	WorldTerrain();
	~WorldTerrain();
	void Init(int seedVar = time(0)); //Input a custom seed or leave it blank for a random seed using time(0)
	void InputNewBoundary(unsigned short width, unsigned short depth, unsigned short height);
	void GenerateRegularWorld();

	//Variables
	noise::module::Cache worldTerrain;
	unsigned short currWidth;
	unsigned short currDepth;
	unsigned short currHeight;

private:
	//Functions
	void GenerateGround();
	void GenerateCaves();
	
	//Variables
	int currSeed;
	noise::module::Const const0;
	noise::module::Const constStone;
	noise::module::Const constSand;
	noise::module::Const constDirt;
	noise::module::Const constGrass;

	//Ground
	noise::module::Gradient groundGradient;
	noise::module::YTurbulence groundBase;
	noise::module::YTurbulence groundPlain;
	noise::module::YTurbulence groundHill;
	noise::module::YTurbulence groundRolling;
	noise::module::HillTurbulence groundSmallMountain;
	noise::module::HillTurbulence groundMediumMountain;
	noise::module::HillTurbulence groundMountain;
	noise::module::HillTurbulence groundSmallChasm;
	noise::module::HillTurbulence groundChasm;

	noise::module::Perlin selPer1;
	noise::module::ScalePoint selSP1;
	noise::module::Select groundMediumTallMtnSelect;

	noise::module::Perlin selPer2;
	noise::module::ScalePoint selSP2;
	noise::module::Select groundMountainSelect;

	noise::module::Perlin selPer3;
	noise::module::ScalePoint selSP3;
	noise::module::Select groundChasmSelect;

	noise::module::Perlin selPer4;
	noise::module::ScalePoint selSP4;
	noise::module::Select groundHillRollingSelect;

	noise::module::Perlin selPer5;
	noise::module::ScalePoint selSP5;
	noise::module::Select groundPlainHillRollingSelect;

	noise::module::Perlin selPer6;
	noise::module::ScalePoint selSP6;
	noise::module::Select groundBaseSelect;

	noise::module::Perlin selPer7;
	noise::module::ScalePoint selSP7;
	noise::module::Select groundBaseMtnSelect;

	noise::module::Perlin selPer8;
	noise::module::ScalePoint selSP8;
	noise::module::Select groundFinalSelect;

	noise::module::Select groundMatSelect;
	noise::module::Cache groundFinal;

	//Caves
	noise::module::RidgedMulti caveShape1;
	noise::module::RidgedMulti caveShape2;
	noise::module::Select caveSelect1;
	noise::module::Select caveSelect2;
	noise::module::Multiply caveMul;
	noise::module::EnhancedTurbulence caveTurb;
	noise::module::ScaleBias caveInvert;
	noise::module::Cache caveFinal;
};