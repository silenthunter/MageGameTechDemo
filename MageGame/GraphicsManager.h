#pragma once

#include <Ogre.h>
#include "WorldTerrain.h"
#include "GameGlobals.h"
#include "GameParser.h"

#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/SimpleVolume.h"
#include <PolyVoxCore/CubicSurfaceExtractor.h>

#include <cegui/CEGUI.h>
#include <cegui/RendererModules/Ogre/CEGUIOgreRenderer.h>

#include "GameTimer.h";

class GraphicsManager
{

public:
	//Functions
	GraphicsManager(void);
	~GraphicsManager(void);
	void InitVoxels(PolyVox::SimpleVolume<VoxelMat>& volData, WorldTerrain wTerra);
	void createSphereInVolume(PolyVox::SimpleVolume<VoxelMat>& volData, float fRadius, PolyVox::Vector3DFloat& v3dVolCenter);
	Ogre::RenderWindow *GetWindow();
	Ogre::SceneNode* GetPlayer();
	Ogre::SceneNode* GetCamera();
	Ogre::Root* GetRoot();
	Ogre::Quaternion GetPlayerRotation();
	void LoadManualObject(PolyVox::SimpleVolume<VoxelMat>& volData, WorldTerrain wTerra);
	void UpdatePhysicsProgress(float progress);
	void CloseGUI();
	//Variables

private:
	//Functions
	void init();
	void SetUpCamera();
	void SetUpWindow(std::string name);

	//Variables
	Ogre::SceneNode *c_sn;
	Ogre::SceneNode *player;
	Ogre::Root *root;
	std::map<std::string,Ogre::RenderWindow*> render_windows;
	Ogre::SceneNode *root_sn;
	Ogre::Camera *c;
	Ogre::RenderWindow *window;
	Ogre::SceneManager *manager;
	Ogre::Viewport *vp;
	std::map<std::string, Ogre::ManualObject*> manualObjects;
	int chunkSize;

	CEGUI::ProgressBar *bar;
	CEGUI::DefaultWindow *rootWin;
	CEGUI::FrameWindow *frame;

	CEGUI::OgreRenderer *mCEGUIrenderer;
};

