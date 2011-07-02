#include <Ogre.h>
#include <OIS.h>
#include <PolyVoxUtil/VolumeChangeTracker.h>
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
using Ogre::Vector3;
using Ogre::Degree;

void ExitMain()
{
	
}

int main(int argc, char* argv[])
{
	atexit(ExitMain); //Call ExitMain if the program quits for any reason

	ParseFile("Data/GameInfo/WorldData.ini", WorldDataMap);
	ParseFile("Data/GameInfo/ItemData.ini", ItemDataMap);
	ParseFile("Data/GameInfo/CharacterData.ini", CharacterDataMap);
	ParseFile("Data/GameInfo/WorldGeneration.ini", WorldGenerationMap);
	ParseFile("Data/GameInfo/WaterData.ini", WaterDataMap);

	ParseIDFile("Data/GameInfo/MaterialID.ini", MaterialIDMap);

	int chunkSize = WorldDataMap["ChunkSize"];

#pragma region Map Generation
	WorldTerrain wTer;
	wTer.Init();
	wTer.GenerateRegularWorld();
	wTer.InputNewBoundary(64, 64, 64);
#pragma endregion

#pragma region GraphicsManager
	GraphicsManager graphicsManager;
	Ogre::SceneNode* player = graphicsManager.GetPlayer();
	Ogre::SceneNode* c_sn = graphicsManager.GetCamera();
	Ogre::Root* root = graphicsManager.GetRoot();

	Ogre::RenderWindow* ogreWindow = graphicsManager.GetWindow();
	size_t hWnd = 0;
	ogreWindow->getCustomAttribute("WINDOW", &hWnd);

	SimpleVolume<VoxelMat> volData(PolyVox::Region(Vector3DInt32(0, 0, 0), Vector3DInt32(wTer.currWidth, wTer.currDepth, wTer.currHeight)));
	graphicsManager.InitVoxels(volData, wTer);
	graphicsManager.LoadManualObject(volData, wTer);
#pragma endregion

#pragma region PhysicsManager
	PhysicsManager physicsManager(&volData, &graphicsManager);
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
		physicsManager.UpdatePlayer(m_Keyboard, m_Mouse, hkQuaternion(playerOri.x, playerOri.y, playerOri.z, playerOri.w));

		//Sync physics character with Ogre
		hkVector4 hkPos = physicsManager.GetPlayerPosition();
		player->setPosition(hkPos(0), hkPos(1), hkPos(2));

		PolyVox::Vector3DFloat voxPos(hkPos(0), hkPos(1), hkPos(2));

		if(m_Keyboard->isKeyDown(OIS::KC_E))
		{
			Ogre::Vector3 forward(0, 0, -1);
			forward = c_sn->_getDerivedOrientation() * forward;
			PolyVox::Vector3DFloat rayDirection(forward.x, forward.y, forward.z);
			rayDirection *= 1000;

			PolyVox::RaycastResult rayResults;
			PolyVox::Raycast<PolyVox::SimpleVolume, VoxelMat> ray(&volData, voxPos, rayDirection, rayResults);
			ray.execute();

			if(rayResults.foundIntersection)
			{
				//rayResults.intersectionVoxel.setX(34);
				//rayResults.intersectionVoxel.setY(3);
				//rayResults.intersectionVoxel.setZ(5);
				PolyVox::Vector3DInt32 chunkNum = rayResults.intersectionVoxel / chunkSize;
				physicsManager.RemoveBlock(chunkNum, rayResults.intersectionVoxel);
				graphicsManager.UpdateChunk(volData, wTer, chunkNum);
			}
		}

		if(count % 50 == 0)
		{
			cout << hkPos(0) << ", " << hkPos(1) << ", " << hkPos(2) << endl;

			printf("FPS: %f\n", 1/elapsed);
			//std::cout << getCurrentChunk(&physicsManager, &world);

			//Set player listener position for sound
			//fslSetListenerPosition(btVec.x(), btVec.y(), btVec.z());
		}

		//Sleep(10);

		//audioUpdate();
		m_Keyboard->copyKeyStates(lastState);
	}

	return 0;
}