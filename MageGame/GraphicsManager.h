#pragma once

#include <Ogre.h>
#include "noiseutils.h"
#include "GameGlobals.h"
#include "GameParser.h"

#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/SimpleVolume.h"

class GraphicsManager
{

public:
	//Functions
	GraphicsManager(void);
	~GraphicsManager(void);
	void InitVoxels(PolyVox::SimpleVolume<PolyVox::MaterialDensityPair44>& volData, utils::NoiseMap& heightMap);
	void createSphereInVolume(PolyVox::SimpleVolume<PolyVox::MaterialDensityPair44>& volData, float fRadius, PolyVox::Vector3DFloat& v3dVolCenter);
	Ogre::RenderWindow *GetWindow();
	void RenderFrame(Ogre::Real timeSinceLastFrame);

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

private:
	//Functions
	void init();
	void SetUpCamera();
	void SetUpWindow(std::string name);

	//Variables
};

