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

#pragma region PhysicsManager
	PhysicsManager physicsManager((viewDist + 0.5f) * chunkSize, chunkSize, worldScale, &volData, &graphicsManager);
#pragma endregion

#pragma region Keyboard and Mouse
	OIS::InputManager *m_InputManager = OIS::InputManager::createInputSystem(hWnd);
	OIS::Keyboard *m_Keyboard = static_cast<OIS::Keyboard*>(m_InputManager->createInputObject(OIS::OISKeyboard, false));
	OIS::Mouse *m_Mouse = static_cast<OIS::Mouse*>(m_InputManager->createInputObject(OIS::OISMouse, false));
#pragma endregion

	graphicsManager.CloseGUI();

	GameTimer timer;
	float speed = 250.f;
	int count = 0;
	char lastState[256];

	while(1)
	{
		count++;
		double elapsed = timer.getElapsedTimeSec();
		root->renderOneFrame(elapsed);
		physicsManager.StepSimulation(elapsed);
		Ogre::WindowEventUtilities::messagePump();

#pragma region Mouse Update
		m_Mouse->capture();
		OIS::MouseState m_MouseState = m_Mouse->getMouseState();
		OIS::Axis xAxis = m_MouseState.X;
		OIS::Axis yAxis = m_MouseState.Y;
		graphicsManager.AdjustCamera(xAxis.rel / -2, yAxis.rel / -2);

#pragma endregion

		m_Keyboard->capture();

		//Update physics character
		Ogre::Quaternion playerOri = c_sn->_getDerivedOrientation();
		physicsManager.UpdatePlayer(m_Keyboard, m_Mouse, hkQuaternion(playerOri.x, playerOri.y, playerOri.z, playerOri.w), elapsed);

		//Sync physics character with Ogre
		hkVector4 hkPos = physicsManager.GetPlayerPosition();
		player->setPosition(hkPos(0), hkPos(1), hkPos(2));
		physicsManager.SyncItemBlocks(graphicsManager.getItemBlocks());

		PolyVox::Vector3DFloat voxPos(hkPos(0), hkPos(1) + 0.4f, hkPos(2));

		if(m_Keyboard->isKeyDown(OIS::KC_F) || (m_Keyboard->isKeyDown(OIS::KC_E) && !lastState[OIS::KC_E]))
		{
			Vector3 forward(0, 0, -1);
			forward = c_sn->_getDerivedOrientation() * forward;
			Vector3DFloat rayDirection(forward.x, forward.y, forward.z);
			rayDirection *= 1000;

			PolyVox::RaycastResult rayResults;
			PolyVox::Raycast<SimpleVolume, VoxelMat> ray(&volData, voxPos, rayDirection, rayResults);
			ray.execute();

			if(rayResults.foundIntersection)
			{
				Vector3DInt32 chunkNum = rayResults.intersectionVoxel / chunkSize;
				VoxelMat blockMat = graphicsManager.RemoveBlock(chunkNum, rayResults.intersectionVoxel);
				physicsManager.RemoveBlock(chunkNum, rayResults.intersectionVoxel);
				graphicsManager.AddItemBlock(rayResults.intersectionVoxel, blockMat, timer.getElapsedTimeSec());
				hkVector4 spawnPos(rayResults.intersectionVoxel.getX(), rayResults.intersectionVoxel.getY(), rayResults.intersectionVoxel.getZ());
				physicsManager.SpawnCube(spawnPos);
			}
		}

#pragma region Player Movement and Chunk Paging
		/*
		//Check player movement
		Vector3DInt32 currPlayerChunk(hkPos(0) / chunkSize, hkPos(1) / chunkSize, hkPos(2) / chunkSize);

		//East-West
		int playerChunkX = currPlayerChunk.getX() - graphicsManager.playerChunk.getX();
		if(playerChunkX > 0) //East
		{
			graphicsManager.MoveEast();
		}
		else if(playerChunkX < 0) //West
		{
			graphicsManager.MoveWest();
		}
			
		//North-South
		int playerChunkZ = currPlayerChunk.getZ() - graphicsManager.playerChunk.getZ();
		if(playerChunkZ > 0) //South
		{
			graphicsManager.MoveSouth();
		}
		else if(playerChunkZ < 0) //North
		{
			graphicsManager.MoveNorth();
		}
		*/
#pragma endregion

		if(count % 50 == 0)
		{
#pragma region Print Statements
			//cout << "Chunk: " << currPlayerChunk.getX() << ", " << currPlayerChunk.getY() << ", " << currPlayerChunk.getZ() << endl;
			cout << "Position: " << hkPos(0) << ", " << hkPos(1) << ", " << hkPos(2) << endl;
			cout << "FPS: " << 1 / elapsed << endl;
			cout << endl;
#pragma endregion

			//Set player listener position for sound
			//fslSetListenerPosition(btVec.x(), btVec.y(), btVec.z());
		}

		//Sleep(10);

		//audioUpdate();
		m_Keyboard->copyKeyStates(lastState);
	}

	return 0;
}