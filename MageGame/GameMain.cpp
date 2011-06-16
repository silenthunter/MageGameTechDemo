#include "NoiseGeneration.h"
#include "noiseutils.h"
#include <Ogre.h>
#include <OIS.h>
#include "GraphicsManager.h"
#include "GameTimer.h"

#include "GameGlobals.h"
#include "GameParser.h"

#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/SimpleVolume.h"
#include <PolyVoxUtil/VolumeChangeTracker.h>
#include <PolyVoxCore/MeshDecimator.h>
#include <PolyVoxCore/SurfaceExtractor.h>

using PolyVox::SimpleVolume;
using PolyVox::MaterialDensityPair44;
using PolyVox::Vector3DInt32;
using Ogre::Vector3;

int main(int argc, char* argv[])
{
	ParseFile("GameFiles/WorldData.ini", WorldDataMap);
	ParseFile("GameFiles/ItemData.ini", ItemDataMap);
	ParseFile("GameFiles/CharacterData.ini", CharacterDataMap);
	ParseFile("GameFiles/WorldGeneration.ini", WorldGenerationMap);
	ParseFile("GameFiles/WaterData.ini", WaterDataMap);

	utils::NoiseMap heightMap = GenerateRandomTerrainMap(-1, 8, 4);
	SimpleVolume<MaterialDensityPair44> volData(PolyVox::Region(Vector3DInt32(0,0,0), Vector3DInt32(heightMap.GetWidth(), 128, heightMap.GetHeight())));

	#pragma region GraphicsManager
	GraphicsManager graphicsManager;

	Ogre::RenderWindow* ogreWindow = graphicsManager.GetWindow();
	size_t hWnd = 0;
	ogreWindow->getCustomAttribute("WINDOW", &hWnd);

	graphicsManager.InitVoxels(volData, heightMap);
	#pragma endregion

	#pragma region Keyboard and Mouse
	OIS::InputManager *m_InputManager = OIS::InputManager::createInputSystem(hWnd);
	OIS::Keyboard *m_Keyboard = static_cast<OIS::Keyboard*>(m_InputManager->createInputObject(OIS::OISKeyboard, false));
	OIS::Mouse *m_Mouse = static_cast<OIS::Mouse*>(m_InputManager->createInputObject(OIS::OISMouse, false));
	#pragma endregion

	GameTimer timer;
	float dis = .5f;
	int count = 0;

	while(1)
	{
		count++;
		double elapsed = timer.getElapsedTimeSec();
		graphicsManager.RenderFrame(elapsed);//Draw frames
		Ogre::WindowEventUtilities::messagePump();
		//dbgdraw->step();

		if(count % 25 == 0)
		{
			printf("FPS: %f\n", 1/elapsed);
			//std::cout << getCurrentChunk(&physicsManager, &world);

			//Set player listener position for sound
			//fslSetListenerPosition(btVec.x(), btVec.y(), btVec.z());
		}

		//Sleep(10);

		//audioUpdate();
	}

	return 0;
}