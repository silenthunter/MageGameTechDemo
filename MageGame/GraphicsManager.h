#pragma once

#include <Ogre.h>
#include <vector>
#include "WorldTerrain.h"
#include "GameGlobals.h"
#include "GameParser.h"

#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/SimpleVolume.h"
#include <PolyVoxCore/CubicSurfaceExtractor.h>

#include <cegui/CEGUI.h>
#include <cegui/RendererModules/Ogre/CEGUIOgreRenderer.h>

#include "GameTimer.h"

struct ItemBlock
{
	Ogre::SceneNode *sn;
	double time;
};

class GraphicsManager
{

public:
	//Functions
	GraphicsManager(int gm_chunkSize,
					float gm_worldScale,
					float gm_blockItemWorldScale,
					int gm_viewDist,
					int gm_verticalMax,
					int gm_verticalChunk,
					PolyVox::SimpleVolume<VoxelMat> *volData,
					WorldTerrain *wTerra,
					GameParser *gm_gameParser);
	~GraphicsManager(void);
	void InitVoxels(int chunkNumX, int chunkNumZ, int xDiff, int zDiff);
	void LoadMO(int chunkNumX, int chunkNumZ, int xDiff, int zDiff);
	void DestroyMO(int chunkNumX, int chunkNumZ, int xDiff, int zDiff);
	VoxelMat RemoveBlock(PolyVox::Vector3DInt32 &chunk, PolyVox::Vector3DInt32 blockPos);
	void UpdateChunk(int x, int y, int z, int xDiff, int zDiff);
	void MoveNorth();
	void MoveSouth();
	void MoveEast();
	void MoveWest();
	void AddItemBlock(PolyVox::Vector3DInt32 blockPos, VoxelMat cubeMat, double time);
	void createSphereInVolume(PolyVox::SimpleVolume<VoxelMat>& volData, float fRadius, PolyVox::Vector3DFloat& v3dVolCenter);
	void UpdatePhysicsProgress(float progress);
	void CloseGUI();
	void AdjustCamera(int xAxis, int yAxis);
	std::vector<ItemBlock> getItemBlocks();

	//Variables
	Ogre::RenderWindow *GetWindow();
	Ogre::SceneNode* GetPlayer();
	Ogre::SceneNode* GetCamera();
	Ogre::Root* GetRoot();
	Ogre::Quaternion GetPlayerRotation();
	PolyVox::Vector3DInt32 playerChunk;

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
	WorldTerrain *wTer;
	PolyVox::SimpleVolume<VoxelMat> *polyVolume;
	GameParser *gameParser;
	std::map<std::string, Ogre::ManualObject*> manualObjects;
	std::vector<ItemBlock> itemBlocks;
	int chunkSize;
	int centerChunk;
	float worldScale;
	float blockItemHalfScale;
	int verticalMax;
	int verticalChunk;
	float oneSixth;
	float twoSixth;
	float threeSixth;
	float fourSixth;
	float fiveSixth;
	CEGUI::ProgressBar *bar;
	CEGUI::DefaultWindow *rootWin;
	CEGUI::FrameWindow *frame;
	CEGUI::OgreRenderer *mCEGUIrenderer;
};