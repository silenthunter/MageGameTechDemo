#pragma once
#include <noise/noise.h>
#include <time.h>
#include <iostream>
#include <random>

#include "noiseutils.h"
#include "GameGlobals.h"

class WorldTerrain
{
public:
	//Functions
	WorldTerrain();
	~WorldTerrain();
	void Init(int seedVar = time(0)); //Input a custom seed or leave it blank for a random seed using time(0)
	void Generate(int width = 16, int height = 8);
	void InputNewBoundary(int width, int height, int depth);
	double ReturnValue(int x, int y, int z);
	//Grab a chunk by inputing the next sequence of numbers.
	/*

		#[This assumes that chunks are in ChunkSize * VerticalMax * ChunkSize]

		#For example, if you are in chunk (4, 5) and you are moving eastward, you should grab the next 3 chunks, 
		which should be chunk (5, 4), (5, 5), and (5, 6)

		TODO: 
		#Need to add overloaded functions such as one that grabs 1x3 or 3x1 chunks based on a direction input 
		and the current chunk you are in

		#Need to add the worldArray as an argument, then convert heightMap's fields (0 through GetWidth() and 
		0 through GetHeight()) which are [-1, 1] to [1, WorldDataMap[VerticalMax]]
	*/
	//void GrabChunk(int xMin, int xMax, int zMin, int zMax);

	//Variables
	module::Turbulence worldTerrain;
	int chunkSize;
	int currWidth;
	int currHeight;
	int currDepth;

private:
	int currSeed;
	module::Perlin groundShape;
	module::ScaleBias groundBias;
	//module::Turbulence groundBase;
	module::RidgedMulti caveShape1;
	module::RidgedMulti caveShape2;
	module::Multiply caveMul;
	module::Turbulence caveTurb;
	module::ScaleBias caveBias;
	module::Invert caveInvert;
};