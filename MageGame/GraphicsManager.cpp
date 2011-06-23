#include "GraphicsManager.h"

using std::string;
using Ogre::ConfigFile;
using Ogre::Vector3;
using Ogre::ManualObject;

using PolyVox::SimpleVolume;
using PolyVox::Vector3DInt32;
using PolyVox::Vector3DFloat;
using PolyVox::SurfaceMesh;
using PolyVox::PositionMaterial;
using PolyVox::CubicSurfaceExtractor;
using PolyVox::Region;

GraphicsManager::GraphicsManager(void)
{
	init();
	chunkSize = WorldDataMap["ChunkSize"];
}

GraphicsManager::~GraphicsManager(void)
{
	delete root;
}

void GraphicsManager::init()
{
#ifdef _DEBUG
	string pluginsFile = "plugins.cfg";
#else
	string pluginsFile = "plugins_release.cfg";
#endif
	string configFile = "ogre.cfg";
	string logFile = "./ogre.log";
	string resourcesFile = "resources.cfg";

	root = new Ogre::Root(pluginsFile, configFile, logFile);
	root->setRenderSystem(root->getRenderSystemByName("Direct3D9 Rendering Subsystem"));
	root->initialise(false);

	ConfigFile cf;
	cf.load(resourcesFile);

	ConfigFile::SectionIterator seci = cf.getSectionIterator();
	string secName, typeName, archName;
	while(seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap * settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator iter;
		for(iter = settings->begin(); iter != settings->end(); iter++)
		{
			typeName = iter->first;
			archName = iter->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}

	SetUpWindow("MageGameTechDemo");
	SetUpCamera();
}

void GraphicsManager::SetUpCamera()
{
	manager = root->createSceneManager(Ogre::ST_GENERIC, "main");
	root_sn = manager->getRootSceneNode();
	manager->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);
	//root_sn->setScale(.1, .1, .1);
	manager->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1)); //Ambient light set here

	//set up camera
	player = manager->createSceneNode("Player");
	c = manager->createCamera("main_camera");
	c->setFOVy(Ogre::Degree(45));
	c->setNearClipDistance(.01);
	c->setFarClipDistance(100);
	c->setAutoAspectRatio(true);

	c->setPosition(Ogre::Vector3(0, 0, 0));

	//attach the camera
	vp = window->addViewport(c);
	c_sn = manager->createSceneNode("main_camera");

	Ogre::Light *l = new Ogre::Light("PlayerLight");
	l->setType(Ogre::Light::LightTypes::LT_POINT);
	l->setDiffuseColour(1, 1, 1);
	c_sn->attachObject(l);

	root_sn->addChild(player);
	player->addChild(c_sn);
	Ogre::SceneNode *cYaw_sn = c_sn->createChildSceneNode("cameraYaw");
	Ogre::SceneNode *cPitch_sn = cYaw_sn->createChildSceneNode("cameraPitch");
	cPitch_sn->attachObject(c);

	player->setPosition(0, 0, 0);
	player->yaw(Ogre::Degree(180));
}

Ogre::RenderWindow* GraphicsManager::GetWindow()
{
	return window;
}

Ogre::SceneNode* GraphicsManager::GetPlayer()
{
	return player;
}

Ogre::SceneNode* GraphicsManager::GetCamera()
{
	return c_sn;
}

Ogre::Root* GraphicsManager::GetRoot()
{
	return root;
}

Ogre::Quaternion GraphicsManager::GetPlayerRotation()
{
	return c_sn->getChild(0)->_getDerivedOrientation();
}

struct s_block
{
	int dir;
	string name;
};

void GraphicsManager::LoadManualObject(SimpleVolume<VoxelMat>& volData, utils::NoiseMap& heightMap)
{
	float scale = 1.f;
	int widthChunks = heightMap.GetWidth() / chunkSize;
	int heightChunks = heightMap.GetHeight() / chunkSize;
	int depthChunks = 4;

	for(int i = 0; i < heightChunks; i++)
	{
		for(int j = 0; j < widthChunks; j++)
		{
			for(int k = 0; k < depthChunks; k++)
			{
				Vector3DInt32 start(j * chunkSize + 1, k * chunkSize + 1, i * chunkSize + 1);
				Vector3DInt32 end((j + 1) * chunkSize, (k + 1) * chunkSize, (i + 1) * chunkSize);

				SurfaceMesh<PositionMaterial> mesh;
				CubicSurfaceExtractor<SimpleVolume, VoxelMat> surfaceExtractor(&volData, Region(start, end), &mesh);
				surfaceExtractor.execute();

				ManualObject *obj = manager->createManualObject(); //Declare the manual object

				//Estimate the vertex count to make it easier
				obj->estimateVertexCount(mesh.getNoOfVertices());
				obj->estimateIndexCount(mesh.getNoOfIndices());

				//Get both the index and vertex data
				vector<uint32_t> vecIndices = mesh.getIndices();
				vector<PositionMaterial> vecVertices = mesh.getVertices();
				
				//Print current chunk information
				char str[50];
				sprintf(str, "%d-%d-%d", i, j, k);

				//Set the object with the render operation and the .material
				obj->begin("VoxelTexture", Ogre::RenderOperation::OT_TRIANGLE_LIST);

				//Work with the vertices
				vector<PositionMaterial>::iterator vecItr;
				for(vecItr = vecVertices.begin(); vecItr != vecVertices.end(); vecItr++)
				{
					PolyVox::Vector3DFloat pos = vecItr->getPosition() * scale;
					pos += Vector3DFloat(j * chunkSize, k * chunkSize, i * chunkSize);
					obj->position(pos.getX(), pos.getY(), pos.getZ());
				}

				//Work with the indices
				vector<uint32_t>::iterator indVec;
				for(indVec = vecIndices.begin(); indVec != vecIndices.end(); indVec++)
				{
					obj->index(*indVec);
				}

				obj->end(); //Done with the manual object

				string strName(str);
				manualObjects[strName] = obj;
				root_sn->attachObject(obj);
			}
			printf("#");
		}
		printf("\n");
	}
}

void GraphicsManager::InitVoxels(PolyVox::SimpleVolume<VoxelMat>& volData, utils::NoiseMap& heightMap)
{
	for(int i = 0; i < heightMap.GetHeight(); i++)
	{
		for(int j = 0; j < heightMap.GetWidth(); j++)
		{
			int depth = (heightMap.GetValue(j, i) + 1) * 64;
			if(depth > 128) depth = 128;
			if(depth < 0) depth = 0;

			for(; depth >= 0; depth--)
			{
				VoxelMat vox = volData.getVoxelAt(Vector3DInt32(j, depth, i));
				vox.setMaterial(1); //Threshold is defaulted at 1 (cannot be changed) so 0 is air and everything else is solid, may end up using something else later
				volData.setVoxelAt(Vector3DInt32(j, depth, i), vox);
			}

			if(i % 32 == 0 && j % 32 == 0)
				printf("#");
		}
		if(i % 32 == 0)
			printf("\n");
	}
}

void GraphicsManager::SetUpWindow(string name)
{
	Ogre::NameValuePairList nvpl;
	nvpl["parentWindowHandle"] = Ogre::StringConverter::toString((size_t)NULL);
	nvpl["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)NULL);

	window = root->createRenderWindow(name, 0, 0, false, &nvpl);
	window->setVisible(true);
	if(render_windows.size() == 0)
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		
	render_windows[name] = window;
	window->setDeactivateOnFocusChange(false);

	window->resize(800,600);
}

std::list<Vector3> GetChunks(Ogre::Vector3& start, Ogre::Vector3& end)
{
	std::list<Vector3> retn;

	int chunkSize = WorldDataMap["ChunkSize"];

	for(int i = start.x / chunkSize; i < end.x / chunkSize; i++)
	{
		for(int j = start.y / chunkSize; j < end.y / chunkSize; j++)
		{
			for(int k = start.z / chunkSize; k < end.z / chunkSize; k++)
			{
				retn.push_back(Vector3(i, j, k));
			}
		}
	}

	return retn;
}

void createSphereInVolume(SimpleVolume<VoxelMat>& volData, float fRadius, Vector3DFloat& v3dVolCenter)
{
	//This vector hold the position of the center of the volume
	//Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);

	//This three-level for loop iterates over every voxel in the volume
	int startZ = v3dVolCenter.getZ() - fRadius; if(startZ < 0) startZ = 0;
	int startY = v3dVolCenter.getY() - fRadius; if(startY < 0) startY = 0;
	int startX = v3dVolCenter.getX() - fRadius; if(startX < 0) startX = 0;

	int height = volData.getHeight();
	int width = volData.getWidth();
	int depth = volData.getDepth();
	for (int z = startZ; z < depth && z < v3dVolCenter.getZ() + fRadius; z++)
	{
		for (int y = startY; y < height && y < v3dVolCenter.getY() + fRadius; y++)
		{
			for (int x = startX; x < width && x < v3dVolCenter.getX() + fRadius; x++)
			{
				//Store our current position as a vector...
				Vector3DFloat v3dCurrentPos(x,y,z);	
				//And compute how far the current position is from the center of the volume
				float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();
				Vector3DFloat dist = v3dCurrentPos - v3dVolCenter;

				//If the current voxel is less than 'radius' units from the center then we make it solid.
				if(fDistToCenter <= fRadius)
				//if(abs(dist.getX()) < fRadius && abs(dist.getY()) < fRadius && abs(dist.getZ()) < fRadius)
				{
					//Our new density value
					uint8_t uDensity = VoxelMat::getMaxDensity();

					//Get the old voxel
					VoxelMat voxel = volData.getVoxelAt(x,y,z);

					//Modify the density
					//voxel.setDensity(uDensity);
					voxel.setDensity(0);

					//Wrte the voxel value into the volume	
					volData.setVoxelAt(x, y, z, voxel);
				}
			}
		}
	}
}