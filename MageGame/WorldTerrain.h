#pragma once
#include <noise/noise.h>
#include <time.h>
#include <iostream>
#include <random>

#include "GameGlobals.h"
#include "Gradient.h"
#include "YTurbulence.h"
#include "EnhancedTurbulence.h"

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
	noise::module::Multiply worldTerrain;
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
	noise::module::Const constMat;

	//Ground
	noise::module::Gradient groundGradient;
	noise::module::YTurbulence groundBase;
	noise::module::YTurbulence groundHill;
	noise::module::YTurbulence groundPlain;
	noise::module::Perlin selectPerlin;
	noise::module::Select groundSelect1;
	noise::module::Select groundSelect2;
	noise::module::Select groundFinalSelect;
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