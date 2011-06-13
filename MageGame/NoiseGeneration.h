#pragma once
#include <noise/noise.h>
#include "noiseutils.h"

//Input a custom seed or use -1 for a random seed using time(0)
void Init(int seedVar);

//Test function
utils::NoiseMap GenerateRandomTerrainMap(int seedVar,int width = 16, int height = 8);

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
void GrabChunk(int xMin, int xMax, int zMin, int zMax);