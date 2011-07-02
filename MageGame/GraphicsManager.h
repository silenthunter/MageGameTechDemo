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
	void InitVoxels(PolyVox::SimpleVolume<VoxelMat> *volData, WorldTerrain *wTerra);
	void LoadManualObject();
	VoxelMat RemoveBlock(PolyVox::Vector3DInt32 &chunk, PolyVox::Vector3DInt32 blockPos);
	void UpdateChunk(PolyVox::Vector3DInt32 chunkNum);
	void createSphereInVolume(PolyVox::SimpleVolume<VoxelMat>& volData, float fRadius, PolyVox::Vector3DFloat& v3dVolCenter);
	void UpdatePhysicsProgress(float progress);
	void CloseGUI();
	void AdjustCamera(int xAxis, int yAxis);

	Ogre::RenderWindow *GetWindow();
	Ogre::SceneNode* GetPlayer();
	Ogre::SceneNode* GetCamera();
	Ogre::Root* GetRoot();
	Ogre::Quaternion GetPlayerRotation();
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
	WorldTerrain* wTer;
	PolyVox::SimpleVolume<VoxelMat>* polyVolume;
	std::map<std::string, Ogre::ManualObject*> manualObjects;
	PolyVox::Vector3DInt32 lowestChunk;
	int chunkSize;
	float worldScale;
	CEGUI::ProgressBar *bar;
	CEGUI::DefaultWindow *rootWin;
	CEGUI::FrameWindow *frame;
	CEGUI::OgreRenderer *mCEGUIrenderer;
};

