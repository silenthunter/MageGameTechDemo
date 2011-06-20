#include "NoiseGeneration.h"
#include "GameGlobals.h"
#include <iostream>
#include <random>
#include <time.h>

using std::cout;
using std::endl;
using std::mt19937;
using std::uniform_int;
using std::uniform_real;
using std::variate_generator;

module::Perlin worldTerrain;
utils::NoiseMap heightMap;
utils::NoiseMapBuilderSphere heightMapBuilder;
int currSeed;

void NoiseGenerationInit(int seedVar)
{
	if(seedVar == -1)
	{
		seedVar = time(0);
	}
	currSeed = seedVar;

	worldTerrain.SetSeed(currSeed);
	worldTerrain.SetOctaveCount(WorldGenerationMap["PerlinOctave"]);
}

utils::NoiseMap GenerateRandomTerrainMap(int width, int height)
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

	heightMapBuilder.SetSourceModule(worldTerrain);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	//heightMapBuilder.EnableSeamless();
	heightMapBuilder.SetDestSize(WorldDataMap["ChunkSize"] * width, WorldDataMap["ChunkSize"] * height);
	heightMapBuilder.SetBounds(-90.0, 90.0, -180.0, 180.0);
	//heightMapBuilder.SetBounds(62.0, 65.0, 62.0, 65.0);
	heightMapBuilder.Build();

	cout << heightMap.GetWidth() << endl;
	cout << heightMap.GetHeight() << endl;
	cout << endl;

	/*
	for(int i = 0; i < heightMap.GetWidth(); ++i)
	{
		for(int j = 0; j < heightMap.GetHeight(); ++j)
		{
			cout << heightMap.GetValue(i, j) << " ";
		}
		cout << endl;
	}
	cout << endl;
	*/

	
	utils::RendererImage renderer;
	utils::Image image;
	renderer.SetSourceNoiseMap(heightMap);
	renderer.SetDestImage(image);
	renderer.ClearGradient();
	renderer.AddGradientPoint (-1.0000, utils::Color (  0,   0, 128, 255)); // deeps
	renderer.AddGradientPoint (-0.2500, utils::Color (  0,   0, 255, 255)); // shallow
	renderer.AddGradientPoint ( 0.0000, utils::Color (  0, 128, 255, 255)); // shore
	renderer.AddGradientPoint ( 0.0625, utils::Color (240, 240,  64, 255)); // sand
	renderer.AddGradientPoint ( 0.1250, utils::Color ( 32, 160,   0, 255)); // grass
	renderer.AddGradientPoint ( 0.3750, utils::Color (224, 224,   0, 255)); // dirt
	renderer.AddGradientPoint ( 0.7500, utils::Color (128, 128, 128, 255)); // rock
	renderer.AddGradientPoint ( 1.0000, utils::Color (255, 255, 255, 255)); // snow
	renderer.EnableLight();
	renderer.SetLightContrast(3.0);
	renderer.SetLightBrightness(2.0);
	renderer.Render();

	utils::WriterBMP writer;
	writer.SetSourceImage(image);
	writer.SetDestFilename("map.bmp");
	writer.WriteDestFile();
	

	return heightMap;
	/*
	heightMapBuilder.SetDestSize(192, 192);
	heightMapBuilder.SetBounds(65.0, 68.0, 62.0, 65.0);
	heightMapBuilder.Build();

	utils::Image image2;
	renderer.SetDestImage(image2);
	renderer.ClearGradient();
	renderer.AddGradientPoint (-1.0000, utils::Color (  0,   0, 128, 255)); // deeps
	renderer.AddGradientPoint (-0.2500, utils::Color (  0,   0, 255, 255)); // shallow
	renderer.AddGradientPoint ( 0.0000, utils::Color (  0, 128, 255, 255)); // shore
	renderer.AddGradientPoint ( 0.0625, utils::Color (240, 240,  64, 255)); // sand
	renderer.AddGradientPoint ( 0.1250, utils::Color ( 32, 160,   0, 255)); // grass
	renderer.AddGradientPoint ( 0.3750, utils::Color (224, 224,   0, 255)); // dirt
	renderer.AddGradientPoint ( 0.7500, utils::Color (128, 128, 128, 255)); // rock
	renderer.AddGradientPoint ( 1.0000, utils::Color (255, 255, 255, 255)); // snow
	renderer.EnableLight();
	renderer.SetLightContrast(3.0);
	renderer.SetLightBrightness(2.0);
	renderer.Render();

	writer.SetSourceImage(image2);
	writer.SetDestFilename("tutorial2.bmp");
	writer.WriteDestFile();
	*/
}

void GrabChunk(int xMin, int xMax, int zMin, int zMax)
{
	heightMapBuilder.SetSourceModule(worldTerrain);
	heightMapBuilder.SetDestNoiseMap(heightMap);

	int destSizeX = (xMax - xMin) * ((int) WorldDataMap["ChunkSize"]);
	int destSizeZ = (zMax - zMin) * ((int) WorldDataMap["ChunkSize"]);

	heightMapBuilder.SetDestSize(destSizeX, destSizeZ);
	heightMapBuilder.SetBounds(xMin, xMax, zMin, zMax);
	heightMapBuilder.Build();

	/*
	for(int i = 0; i < heightMap.GetWidth(); ++i)
	{
		for(int j = 0; j < heightMap.GetHeight(); ++j)
		{
			cout << heightMap.GetValue(i, j) << " ";
		}
		cout << endl;
	}
	cout << endl;
	*/
}