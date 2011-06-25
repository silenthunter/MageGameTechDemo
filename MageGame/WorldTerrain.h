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
	void GenerateRegularWorld();
	void InputNewBoundary(int width, int height, int depth);

	//Variables
	noise::module::Multiply worldTerrain;
	int chunkSize;
	int currWidth;
	int currHeight;
	int currDepth;

private:
	//Variables
	int currSeed;

	//Ground
	noise::module::Const const0;
	noise::module::Const const1;
	noise::module::Gradient groundGradiant;
	noise::module::YTurbulence groundTurb;
	noise::module::Select groundSelect;

	//Caves
	float caveLowerBound;
	unsigned int caveOctaveCount;
	unsigned int caveFrequency;
	noise::module::RidgedMulti caveShape1;
	noise::module::RidgedMulti caveShape2;
	noise::module::Select caveSelect1;
	noise::module::Select caveSelect2;
	noise::module::Multiply caveMul;
	noise::module::EnhancedTurbulence caveTurb;
	noise::module::ScaleBias caveInvert;
};