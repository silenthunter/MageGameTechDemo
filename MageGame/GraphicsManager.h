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
	GraphicsManager(int gm_chunkSize, float gm_worldScale, int gm_viewDist, int gm_verticalMax, PolyVox::SimpleVolume<VoxelMat> *volData, WorldTerrain *wTerra);
	~GraphicsManager(void);
	void InitVoxels(int minX, int minY, int minZ, int maxX, int maxY, int maxZ);
	void LoadManualObjects(int minX, int minY, int minZ, int maxX, int maxY, int maxZ);
	void LoadSingleMO(int i, int j, int k, int iDiff, int kDiff);
	void DestroyMO(PolyVox::Vector3DInt32 chunkNum);
	VoxelMat RemoveBlock(PolyVox::Vector3DInt32 &chunk, PolyVox::Vector3DInt32 blockPos);
	void UpdateChunk(PolyVox::Vector3DInt32 chunkNum);
	void MoveNorth();
	void MoveSouth();
	void MoveEast();
	void MoveWest();
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
	PolyVox::Vector3DInt32 playerChunk;
	int chunkSize;
	int centerChunk;
	float worldScale;
	int verticalMax;
	CEGUI::ProgressBar *bar;
	CEGUI::DefaultWindow *rootWin;
	CEGUI::FrameWindow *frame;
	CEGUI::OgreRenderer *mCEGUIrenderer;
};

