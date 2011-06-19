#include "GraphicsManager.h"

using std::string;
using Ogre::ConfigFile;
using Ogre::Vector3;
using Ogre::ManualObject;

using PolyVox::SimpleVolume;
using PolyVox::MaterialDensityPair44;
using PolyVox::Vector3DInt32;
using PolyVox::Vector3DFloat;
using PolyVox::SurfaceMesh;
using PolyVox::PositionMaterialNormal;
using PolyVox::CubicSurfaceExtractorWithNormals;
using PolyVox::Region;

GraphicsManager::GraphicsManager(void)
{
	init();
	chunkSize = WorldDataMap["ChunkSize"];
}

GraphicsManager::~GraphicsManager(void)
{
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
	root_sn->setScale(.1, .1, .1);
	manager->setAmbientLight(Ogre::ColourValue(1, 1, 1)); //Ambient light set here

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

void GraphicsManager::LoadManualObject(PolyVox::SimpleVolume<PolyVox::MaterialDensityPair44>& volData, utils::NoiseMap& heightMap)
{
	float scale = 1.f;
	int widthChunks = heightMap.GetWidth() / chunkSize;
	int heightChunks = heightMap.GetHeight() / chunkSize;
	int depthChunks = 4;
	bool sortafirstLoop = true;
	bool firstLoop = false;

	for(int i = 0; i < heightChunks; i++)
	{
		for(int j = 0; j < widthChunks; j++)
		{

			for(int k = 0; k < depthChunks; k++)
			{
				Vector3DInt32 start(j * chunkSize, k * chunkSize, i * chunkSize);
				Vector3DInt32 end((j + 1) * chunkSize, (k + 1) * chunkSize, (i + 1) * chunkSize);

				SurfaceMesh<PositionMaterialNormal> mesh;
				CubicSurfaceExtractorWithNormals<SimpleVolume, MaterialDensityPair44> surfaceExtractor(&volData, Region(start, end), &mesh);
				surfaceExtractor.execute();

				ManualObject *obj = manager->createManualObject();

				size_t numVertices = mesh.getNoOfVertices();
				size_t numIndices = mesh.getNoOfIndices();

				obj->estimateVertexCount(numVertices);
				obj->estimateIndexCount(numIndices);

				//std::vector<uint32_t> vecIndices = mesh.getIndices();
				//std::vector<PositionMaterialNormal> vecVertices = mesh.getVertices();

				if(numVertices > 0) firstLoop = true;

				/*
				hkpExtendedMeshShape::TrianglesSubpart subPart;
				float* vert;
				unsigned long *ind;
				hkpExtendedMeshShape *shp;

				if(firstLoop)
				{
					shp = new hkpExtendedMeshShape();
					subPart.m_numVertices = vecVertices.size() * 3;
					subPart.m_numTriangleShapes = vecIndices.size() / 3;
					subPart.m_vertexStriding = sizeof(float) * 3;
					subPart.m_indexStriding = sizeof(unsigned long) * 3;
					vert = new float[subPart.m_numVertices];
					ind = new unsigned long[subPart.m_numTriangleShapes * 3];

				}
				*/

				char str[50];

				sprintf(str, "%d-%d-%d", i, j, k);

				obj->begin("ColouredCubicVoxel", Ogre::RenderOperation::OT_TRIANGLE_LIST);

				// vertices
				const vector<PositionMaterialNormal>& vVertices = mesh.getVertices();
				for(size_t i = 0; i < numVertices; i++)
				{
					Vector3DFloat pos = vVertices[i].getPosition() * scale;
					pos += Vector3DFloat(j * chunkSize, k * chunkSize, i * chunkSize);
					obj->position(pos.getX(), pos.getY(), pos.getZ());
					obj->normal(vVertices[i].getNormal().getX(), vVertices[i].getNormal().getY(), vVertices[i].getNormal().getZ());
				}

				// indices
				const std::vector<uint32_t>& vIndices = mesh.getIndices();
				for(size_t i = 0; i < numIndices; i++)
				{
					obj->index(vIndices[i]);
				}

				/*
				float vecCnt = 0;
				std::vector<PositionMaterialNormal>::iterator vecItr;
				float texX = 0.f, texY = 0.f;
				for(vecItr = vecVertices.begin(); vecItr != vecVertices.end(); vecItr++, vecCnt++)
				{
					PolyVox::Vector3DFloat pos = (*vecItr).getPosition() * scale;
					pos += Vector3DFloat(j * chunkSize, k * chunkSize, i* chunkSize);
					obj->position(pos.getX(), pos.getY(), pos.getZ());
					obj->textureCoord(vecCnt / vecVertices.size(), vecCnt / vecVertices.size());
					obj->normal((*vecItr).getNormal().getX(), (*vecItr).getNormal().getY(), (*vecItr).getNormal().getZ());
				*/
					/*
					if(firstLoop)
					{
						vert[(int)vecCnt * 3 + 0] = pos.getX();
						vert[(int)vecCnt * 3 + 1] = pos.getY();
						vert[(int)vecCnt * 3 + 2] = pos.getZ();
					}
					*/
				/*
				}

				std::vector<uint32_t>::iterator indVec;
				int indCnt = 0;
				for(indVec = vecIndices.begin(); indVec != vecIndices.end(); indVec++, indCnt++)
				{
					obj->index(*indVec);
					//if(firstLoop) ind[indCnt] = *indVec;
				}
				*/
				obj->end();

				string strName(str);
				manualObjects[strName] = obj;

				root_sn->attachObject(obj);

				/*
				if(firstLoop)
				{
					subPart.m_vertexBase = vert;
					subPart.m_indexBase = ind;
					subPart.m_stridingType = hkpExtendedMeshShape::INDICES_INT32;
					shp->addTrianglesSubpart(subPart);
					sortafirstLoop = false;

					hkpMoppCompilerInput mci;

					hkpRigidBodyCinfo info;

					hkpMoppCode *code = hkpMoppUtility::buildCode(shp, mci);

					hkpMoppBvTreeShape* moppShape = new hkpMoppBvTreeShape(shp, code);

					hkVector4 dimensions(50.f, 2.f, 50.f);
					hkpConvexShape *shape = new hkpBoxShape(dimensions, 0);

					//hkpBvTreeShape *sshp = new hkpBvTreeShape();

					//hkpShape* shapeUsed = shp;

					info.m_motionType = hkpMotion::MOTION_FIXED;
					info.m_shape = moppShape;
					info.m_position = hkVector4(0, 0, 0);

					hkpRigidBody *body = new hkpRigidBody(info);

					world->lock();
					world->addEntity(body);
					body->removeReference();
					shp->removeReference();

					world->unlock();
				}
				*/

				firstLoop = false;
			}

			printf("#");
		}
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

void GraphicsManager::InitVoxels(SimpleVolume<MaterialDensityPair44>& volData, utils::NoiseMap& heightMap)
{
	for(int i = 0; i < heightMap.GetHeight(); i++)
	{
		for(int j = 0; j < heightMap.GetWidth(); j++)
		{
			int depth = heightMap.GetValue(j, i) * 64;
			if(depth >= 64) depth = 64;
			if(depth < 0) depth = 0;

			depth += 64;

			for(; depth >= 0; depth--)
			{
				PolyVox::MaterialDensityPair44 vox = volData.getVoxelAt(Vector3DInt32(j, depth, i));
				vox.setDensity(MaterialDensityPair44::getMaxDensity());
				volData.setVoxelAt(Vector3DInt32(j, depth, i), vox);
			}
			if(i % 32 == 0 && j % 32 == 0)
				printf("#");
		}
		if(i % 32 == 0)
			printf("\n");
	}
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

void createSphereInVolume(SimpleVolume<MaterialDensityPair44>& volData, float fRadius, Vector3DFloat& v3dVolCenter)
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
					uint8_t uDensity = MaterialDensityPair44::getMaxDensity();

					//Get the old voxel
					MaterialDensityPair44 voxel = volData.getVoxelAt(x,y,z);

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