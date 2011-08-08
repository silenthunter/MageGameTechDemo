/**
@mainpage MageGame
A description of the project will go here
*/

#include <Ogre.h>
#include <OIS.h>
#include <PolyVoxUtil/VolumeChangeTracker.h>
#include <PolyVoxCore/SimpleVolume.h>
#include <PolyVoxCore/MeshDecimator.h>
#include <PolyVoxCore/Raycast.h>
#include <Common/Base/hkBase.h>

#include "GraphicsManager.h"
#include "PhysicsManager.h"
#include "WorldTerrain.h"
#include "GameTimer.h"
#include "GameGlobals.h"
#include "GameParser.h"

using PolyVox::SimpleVolume;
using PolyVox::Vector3DInt32;
using PolyVox::Vector3DFloat;
using Ogre::Vector3;
using Ogre::Degree;

void ExitMain()
{
	
}

int main(int argc, char* argv[])
{
	atexit(ExitMain); //Call ExitMain if the program quits for any reason

#pragma region GameParser
	GameParser gameParser("Data/GameInfo/", "World1/ChunkData/");

	gameParser.ParseFile("WorldData.ini", WorldDataMap);
	gameParser.ParseFile("ItemData.ini", ItemDataMap);
	gameParser.ParseFile("CharacterData.ini", CharacterDataMap);
	gameParser.ParseFile("WorldGeneration.ini", WorldGenerationMap);
	gameParser.ParseFile("WaterData.ini", WaterDataMap);

	gameParser.ParseIDFile("MaterialID.ini", MaterialIDMap);

	int chunkSize = WorldDataMap["ChunkSize"];
	float worldScale = WorldDataMap["WorldScale"];
	float blockItemWorldScale = WorldDataMap["BlockItemWorldScale"];
	int viewDist = ((1.0f / worldScale) * WorldDataMap["ViewDistance"] + 0.5);

	gameParser.SetChunkSize(chunkSize);
#pragma endregion

#pragma region Map Generation
	WorldTerrain wTer;
	wTer.Init();
	wTer.GenerateRegularWorld();
	int horizontalChunkSize = ((viewDist * 2) + 1) * chunkSize;
	int verticalMax = WorldDataMap["VerticalMax"];
	int verticalChunk = verticalMax / chunkSize;
	wTer.InputNewBoundary(horizontalChunkSize, verticalMax, horizontalChunkSize);
#pragma endregion

#pragma region GraphicsManager
	SimpleVolume<VoxelMat> volData(PolyVox::Region(Vector3DInt32(0, 0, 0), Vector3DInt32(wTer.currWidth, wTer.currDepth, wTer.currHeight)));

	gameParser.SetPolyVolume(&volData);
	gameParser.SetVerticalChunk(verticalChunk);

	GraphicsManager graphicsManager(chunkSize, worldScale, blockItemWorldScale, viewDist, verticalMax, verticalChunk, &volData, &wTer, &gameParser);
	Ogre::SceneNode* player = graphicsManager.GetPlayer();
	Ogre::SceneNode* c_sn = graphicsManager.GetCamera();
	Ogre::Root* root = graphicsManager.GetRoot();

	Ogre::RenderWindow* ogreWindow = graphicsManager.GetWindow();
	size_t hWnd = 0;
	ogreWindow->getCustomAttribute("WINDOW", &hWnd);
#pragma endregion

#pragma region Keyboard and Mouse
	OIS::InputManager *m_InputManager = OIS::InputManager::createInputSystem(hWnd);
	OIS::Keyboard *m_Keyboard = static_cast<OIS::Keyboard*>(m_InputManager->createInputObject(OIS::OISKeyboard, false));
	OIS::Mouse *m_Mouse = static_cast<OIS::Mouse*>(m_InputManager->createInputObject(OIS::OISMouse, false));
#pragma endregion

	graphicsManager.CloseGUI();

	GameTimer timer;
	float speed = 100.f;
	int count = 0;
	char lastState[256];

	while(1)
	{
		count++;
		double elapsed = timer.getElapsedTimeSec();
		root->renderOneFrame(elapsed);
		Ogre::WindowEventUtilities::messagePump();

#pragma region Mouse Update
		m_Mouse->capture();
		OIS::MouseState m_MouseState = m_Mouse->getMouseState();
		OIS::Axis xAxis = m_MouseState.X;
		OIS::Axis yAxis = m_MouseState.Y;
		graphicsManager.AdjustCamera(xAxis.rel / -2, yAxis.rel / -2);

#pragma endregion

		m_Keyboard->capture();

		if(m_Keyboard->isKeyDown(OIS::KC_W))
		{
			Ogre::Vector3 vec(0, 0, -speed * elapsed);
			vec = graphicsManager.c_sn->_getDerivedOrientation() * vec;
			graphicsManager.player->translate(vec);
		}
		if(m_Keyboard->isKeyDown(OIS::KC_S))
		{
			Ogre::Vector3 vec(0, 0, speed * elapsed);
			vec = graphicsManager.c_sn->_getDerivedOrientation() * vec;
			graphicsManager.player->translate(vec);
		}
		if(m_Keyboard->isKeyDown(OIS::KC_A))
		{
			Ogre::Vector3 vec(-speed * elapsed, 0, 0);
			vec = graphicsManager.c_sn->_getDerivedOrientation() * vec;
			graphicsManager.player->translate(vec);
		}
		if(m_Keyboard->isKeyDown(OIS::KC_D))
		{
			Ogre::Vector3 vec(speed * elapsed, 0, 0);
			vec = graphicsManager.c_sn->_getDerivedOrientation() * vec;
			graphicsManager.player->translate(vec);
		}

		if(count % 50 == 0)
		{
#pragma region Print Statements
			cout << "FPS: " << 1 / elapsed << endl;
#pragma endregion

			//Set player listener position for sound
			//fslSetListenerPosition(btVec.x(), btVec.y(), btVec.z());
		}

		m_Keyboard->copyKeyStates(lastState);
	}

	return 0;
}