#pragma once
#include <noise/noise.h>
#include <time.h>
#include <iostream>
#include <random>

#include "GameGlobals.h"
#include "Gradient.h"
#include "YTurbulence.h"

class WorldTerrain
{
public:
	//Functions
	WorldTerrain();
	~WorldTerrain();
	void Init(int seedVar = time(0)); //Input a custom seed or leave it blank for a random seed using time(0)
	void Generate();
	void InputNewBoundary(int width, int height, int depth);
	double ReturnValue(int x, int y, int z);

	//Variables
	noise::module::Select worldTerrain;
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

	noise::module::RidgedMulti caveShape1;
	noise::module::RidgedMulti caveShape2;
	noise::module::Select caveSelect1;
	noise::module::Select caveSelect2;
	noise::module::Multiply caveMul;
	noise::module::Turbulence caveTurb;
	noise::module::Invert caveInvert;
};