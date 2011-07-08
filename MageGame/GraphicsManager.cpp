#include "GraphicsManager.h"

#define NUM_TEX_PER_ROW 4
#define NUM_NONTEX_MATERIALS 1
#define TEX_WIDTH_NORMALIZED (1.0 / NUM_TEX_PER_ROW)

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

GraphicsManager::GraphicsManager(int gm_chunkSize, float gm_worldScale, int gm_viewDist, int gm_verticalMax, PolyVox::SimpleVolume<VoxelMat> *volData, WorldTerrain *wTerra, GameParser *gm_gameParser)
{
	init();
	polyVolume = volData;
	wTer = wTerra;
	gameParser = gm_gameParser;
	chunkSize = gm_chunkSize;
	worldScale = gm_worldScale;
	centerChunk = gm_viewDist + 1;
	playerChunk.setX(centerChunk);
	playerChunk.setZ(centerChunk);
	verticalMax = gm_verticalMax;

	int horizontalChunk = gm_viewDist * 2 + 1;
	int verticalChunk = gm_verticalMax / gm_chunkSize;
	for(int i = 0; i < horizontalChunk; ++i)
	{
		for(int j = 0; j < verticalChunk; ++j)
		{
			for(int k = 0; k < horizontalChunk; ++k)
			{
				InitVoxels(Vector3DInt32(i, j, k));
			}
		}
	}
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

	//Set up menu
	mCEGUIrenderer = &CEGUI::OgreRenderer::bootstrapSystem(*window);
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	//CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	rootWin = (CEGUI::DefaultWindow*)winMgr.createWindow("DefaultWindow", "Root");
	CEGUI::System::getSingleton().setGUISheet(rootWin);

	frame = (CEGUI::FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "Demo Window");
	rootWin->addChildWindow(frame);

    frame->setPosition(CEGUI::UVector2(cegui_reldim(0.25f), cegui_reldim( 0.25f)));
    frame->setSize(CEGUI::UVector2(cegui_reldim(0.5f), cegui_reldim( 0.5f)));

	frame->setMaxSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim( 1.0f)));
    frame->setMinSize(CEGUI::UVector2(cegui_reldim(0.1f), cegui_reldim( 0.1f)));

	frame->setText("Loading Graphics");

	bar = (CEGUI::ProgressBar*)winMgr.createWindow("TaharezLook/ProgressBar", "Pbar");

	bar->setProgress(0);
	bar->setSize(CEGUI::UVector2(CEGUI::UDim(.8f, 0), CEGUI::UDim(0.1f, 0)));
	bar->setPosition(CEGUI::UVector2(CEGUI::UDim(.1f, 0), CEGUI::UDim(.5f, 0)));
	frame->addChildWindow(bar);

}

void GraphicsManager::SetUpCamera()
{
	manager = root->createSceneManager(Ogre::ST_GENERIC, "main");
	root_sn = manager->getRootSceneNode();
	manager->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);
	manager->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1)); //Ambient light set here

	//set up camera
	player = manager->createSceneNode("Player");
	c = manager->createCamera("main_camera");
	c->setFOVy(Ogre::Degree(45));
	c->setNearClipDistance(.01);
	c->setFarClipDistance(100);
	c->setAutoAspectRatio(true);

	c->setPosition(Ogre::Vector3(0, .4f, 0));//up .4f because that's the physics capsule's offset

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

void GraphicsManager::UpdatePhysicsProgress(float progress)
{
	bar->setProgress(progress);
	root->renderOneFrame(1.f);
	Ogre::WindowEventUtilities::messagePump();
}

void GraphicsManager::CloseGUI()
{
	rootWin->setVisible(false);
}

void GraphicsManager::InitVoxels(PolyVox::Vector3DInt32 chunkNum)
{
	float regionWidth = 1.0;
	float regionHeight = 1.0;
	float regionDepth = 1.0;

	int chunkNumX = chunkNum.getX();
	int chunkNumY = chunkNum.getY();
	int chunkNumZ = chunkNum.getZ();

	int chunkX = (chunkNumX + playerChunk.getX() - centerChunk) * regionWidth;
	int chunkY = chunkNumY * regionDepth;
	int chunkZ = (chunkNumZ + playerChunk.getZ() - centerChunk) * regionHeight;

	int xOffset = chunkNumX * chunkSize;
	int yOffset = chunkNumY * chunkSize;
	int zOffset = chunkNumZ * chunkSize;

	for(int x = 0; x < chunkSize; x++)
	{
		for(int z = 0; z < chunkSize; z++)
		{
			for(int y = 0; y < chunkSize; y++)
			{
				double nx = (double)x / chunkSize;
				double ny = (double)y / chunkSize;
				double nz = (double)z / chunkSize;

				nx = chunkX + nx * regionWidth;
				ny = chunkY + ny * regionHeight;
				nz = chunkZ + nz * regionDepth;

				double v = wTer->worldTerrain.GetValue(nx, ny, nz);
                if(v > 0)
				{
					VoxelMat vox;
					vox.setMaterial(v);
					polyVolume->setVoxelAt(x + xOffset, y + yOffset, z + zOffset, vox);
				}
			}
		}
	}
}

void GraphicsManager::LoadManualObjects(int minX, int minY, int minZ, int maxX, int maxY, int maxZ)
{
	unsigned int widthChunks = maxX / chunkSize;
	unsigned int depthChunks = maxY / chunkSize;
	unsigned int heightChunks = maxZ / chunkSize;
	int iDiff = centerChunk - playerChunk.getX();
	int kDiff = centerChunk - playerChunk.getZ();
	double elapsedTotal = 0;
	GameTimer timer;

	float heightXWidth = heightChunks * widthChunks;

	for(int i = minX; i < widthChunks; ++i)
	{
		for(int j = minY; j < depthChunks; ++j)
		{
			for(int k = minZ; k < heightChunks; ++k)
			{
				elapsedTotal += timer.getElapsedTimeSec();

				if(elapsedTotal >= .1f)
				{
					float progress = (float)(k * widthChunks + i) / heightXWidth;
					bar->setProgress(progress);

					root->renderOneFrame(1.f);
					Ogre::WindowEventUtilities::messagePump();
					elapsedTotal = 0;
				}

				LoadSingleMO(i, j, k, iDiff, kDiff);
			}
			printf("#");
		}
		printf("\n");
	}

	frame->setText("Loading Physics");
	bar->setProgress(1.f);
	root->renderOneFrame(1.f);
	Ogre::WindowEventUtilities::messagePump();
}

void GraphicsManager::LoadSingleMO(int i, int j, int k, int iDiff, int kDiff)
{
	unsigned int iChunkSize = i * chunkSize;
	unsigned int jChunkSize = j * chunkSize;
	unsigned int kChunkSize = k * chunkSize;
	unsigned int chunkAddition = chunkSize - 1;

	Vector3DInt32 start(iChunkSize, jChunkSize, kChunkSize);
	Vector3DInt32 end(iChunkSize + chunkAddition, jChunkSize + chunkAddition, kChunkSize + chunkAddition);

	SurfaceMesh<PositionMaterial> mesh;
	CubicSurfaceExtractor<SimpleVolume, VoxelMat> surfaceExtractor(polyVolume, Region(start, end), &mesh);
	surfaceExtractor.execute();

	ManualObject *obj = manager->createManualObject(); //Declare the manual object
	obj->setDynamic(true);

	//Estimate the vertex count to make it easier
	obj->estimateVertexCount(mesh.getNoOfVertices());
	obj->estimateIndexCount(mesh.getNoOfIndices());

	//Get both the index and vertex data
	const vector<uint32_t> &vecIndices = mesh.getIndices();
	const vector<PositionMaterial> &vecVertices = mesh.getVertices();
				
	//Print current chunk information
	char str[50];
	sprintf(str, "%d_%d_%d", i + iDiff, j, k + kDiff);

	//Set the object with the render operation and the .material
	obj->begin("VoxelTexture", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	//Work with the vertices
				
	Vector3DFloat posOffset = Vector3DFloat(iChunkSize, jChunkSize, kChunkSize) * worldScale;
	for(vector<PositionMaterial>::const_iterator vecItr = vecVertices.begin(); vecItr != vecVertices.end(); vecItr++)
	{
		Vector3DFloat pos = vecItr->getPosition() * worldScale;
		pos += posOffset;
		obj->position(pos.getX(), pos.getY(), pos.getZ());
		Ogre::ColourValue val;
		VoxelMat vMat = vecItr->getMaterial();
		uint16_t mat = vMat.getMaterial() - NUM_NONTEX_MATERIALS;
		/*
		r = u coords where the texture starts
		g = v coords where the texture starts
		b = direction of the block with 0.0, 0.25, 0.5, 0.75 as North, East, South, West
		a = maybe used to flag that one of the faces uses a different texture?
		*/
		val.r = mat % NUM_TEX_PER_ROW * TEX_WIDTH_NORMALIZED;
		val.g = mat / NUM_TEX_PER_ROW * TEX_WIDTH_NORMALIZED;
		val.b = 0.0f;
		val.a = 1.0f;
		obj->colour(val);
	}

	//Work with the indices
	for(vector<uint32_t>::const_iterator indVec = vecIndices.begin(); indVec != vecIndices.end(); indVec++)
	{
		obj->index(*indVec);
	}

	obj->end(); //Done with the manual object

	string strName(str);
	manualObjects.insert(std::map<string, ManualObject*>::value_type(str, obj));
	root_sn->attachObject(obj);
}

void GraphicsManager::DestroyMO(PolyVox::Vector3DInt32 chunkNum)
{
	char str[50];
	sprintf(str, "%d_%d_%d", chunkNum.getX(), chunkNum.getY(), chunkNum.getZ());
	manualObjects.erase(str);
}

VoxelMat GraphicsManager::RemoveBlock(PolyVox::Vector3DInt32 &chunk, PolyVox::Vector3DInt32 blockPos)
{
	VoxelMat vox = polyVolume->getVoxelAt(blockPos);
	polyVolume->setVoxelAt(blockPos, 0);

	UpdateChunk(chunk);
	
	int posX = blockPos.getX();
	int posY = blockPos.getY();
	int posZ = blockPos.getZ();
	int chunkX = chunk.getX();
	int chunkY = chunk.getY();
	int chunkZ = chunk.getZ();
	unsigned short chunkSizeEnd = chunkSize - 1;

	if(posX != 0 && posX % chunkSize == 0)
	{
		UpdateChunk(Vector3DInt32(chunkX - 1, chunkY, chunkZ));
	}
	else if(posX != wTer->currWidth - 1 && posX % chunkSize == chunkSizeEnd)
	{
		UpdateChunk(Vector3DInt32(chunkX + 1, chunkY, chunkZ));
	}

	if(posY != 0 && posY % chunkSize == 0)
	{
		UpdateChunk(Vector3DInt32(chunkX, chunkY - 1, chunkZ));
	}
	else if(posY != wTer->currDepth - 1 && posY % chunkSize == chunkSizeEnd)
	{
		UpdateChunk(Vector3DInt32(chunkX, chunkY + 1, chunkZ));
	}

	if(posZ != 0 && posZ % chunkSize == 0)
	{
		UpdateChunk(Vector3DInt32(chunkX, chunkY, chunkZ - 1));
	}
	else if(posZ != wTer->currHeight - 1 && posZ % chunkSize == chunkSizeEnd)
	{
		UpdateChunk(Vector3DInt32(chunkX, chunkY, chunkZ + 1));
	}

	return vox;
}

void GraphicsManager::UpdateChunk(Vector3DInt32 chunkNum)
{
	int i = chunkNum.getX();
	int j = chunkNum.getY();
	int k = chunkNum.getZ();

	int iDiff = playerChunk.getX() - centerChunk;
	int kDiff = playerChunk.getZ() - centerChunk;

	char str[50];
	sprintf(str, "%d_%d_%d", i + iDiff, j, k + kDiff);

	unsigned int iChunkSize = i * chunkSize;
	unsigned int jChunkSize = j * chunkSize;
	unsigned int kChunkSize = k * chunkSize;
	unsigned short chunkAddition = chunkSize - 1;

	Vector3DInt32 start(iChunkSize, jChunkSize, kChunkSize);
	Vector3DInt32 end(iChunkSize + chunkAddition, jChunkSize + chunkAddition, kChunkSize + chunkAddition);

	SurfaceMesh<PositionMaterial> mesh;
	CubicSurfaceExtractor<SimpleVolume, VoxelMat> surfaceExtractor(polyVolume, Region(start, end), &mesh);
	surfaceExtractor.execute();

	ManualObject *obj = manualObjects[str];

	obj->estimateVertexCount(mesh.getNoOfVertices());
	obj->estimateIndexCount(mesh.getNoOfIndices());

	const vector<uint32_t> &vecIndices = mesh.getIndices();
	const vector<PositionMaterial> &vecVertices = mesh.getVertices();

	obj->beginUpdate(0);

	Vector3DFloat posOffset = Vector3DFloat(iChunkSize, jChunkSize, kChunkSize) * worldScale;
	for(vector<PositionMaterial>::const_iterator vecItr = vecVertices.begin(); vecItr != vecVertices.end(); vecItr++)
	{
		Vector3DFloat pos = vecItr->getPosition() * worldScale;
		pos += posOffset;
		obj->position(pos.getX(), pos.getY(), pos.getZ());
	}

	for(vector<uint32_t>::const_iterator indVec = vecIndices.begin(); indVec != vecIndices.end(); indVec++)
	{
		obj->index(*indVec);
	}

	obj->end();
}

void GraphicsManager::MoveNorth()
{
	int xDiff = playerChunk.getX() - centerChunk;
	int zDiff = playerChunk.getZ() - centerChunk;

	int horizontalChunk = centerChunk * 2 + 1;
	int horizontalSize = horizontalChunk * chunkSize;
	int size = horizontalSize * verticalMax;

	gameParser->PlayerMoveStore(xDiff, zDiff, horizontalChunk, verticalMax / chunkSize, 0);

	memmove(polyVolume + size, polyVolume, size);

	playerChunk.setZ(playerChunk.getZ() - 1);
}

void GraphicsManager::MoveSouth()
{
	int xDiff = playerChunk.getX() - centerChunk;
	int zDiff = playerChunk.getZ() - centerChunk;

	int horizontalChunk = centerChunk * 2 + 1;
	int horizontalSize = horizontalChunk * chunkSize;
	int start = horizontalSize * verticalMax;
	int size = (horizontalSize - 1) * horizontalSize * verticalMax;

	gameParser->PlayerMoveStore(xDiff, zDiff, horizontalChunk, verticalMax / chunkSize, 2);

	memmove(polyVolume, polyVolume + start, size);

	playerChunk.setZ(playerChunk.getZ() + 1);
}

void GraphicsManager::MoveEast()
{
	int xDiff = playerChunk.getX() - centerChunk;
	int zDiff = playerChunk.getZ() - centerChunk;

	int horizontalChunk = (centerChunk * 2 + 1);
	int horizontalSize = horizontalChunk * chunkSize;
	int horizontalSlice = horizontalSize * verticalMax;
	int start = chunkSize;
	int size = horizontalSize - chunkSize;
	int verticalChunk = verticalMax / chunkSize;

	gameParser->PlayerMoveStore(xDiff, zDiff, horizontalChunk, verticalChunk, 1);

	for(int i = 0; i < horizontalChunk; ++i)
	{
		for(int j = 0; j < verticalChunk; ++j)
		{
			memmove(polyVolume + (horizontalSlice * i + horizontalSize * j), polyVolume + (horizontalSlice * i + horizontalSize * j + start), size);
		}
	}

	playerChunk.setX(playerChunk.getX() + 1);
}

void GraphicsManager::MoveWest()
{
	int xDiff = playerChunk.getX() - centerChunk;
	int zDiff = playerChunk.getZ() - centerChunk;

	int horizontalChunk = (centerChunk * 2 + 1);
	int horizontalSize = horizontalChunk * chunkSize;
	int horizontalSlice = horizontalSize * verticalMax;
	int end = chunkSize;
	int size = horizontalSize - chunkSize;
	int verticalChunk = verticalMax / chunkSize;

	gameParser->PlayerMoveStore(xDiff, zDiff, horizontalChunk, verticalChunk, 3);

	for(int i = 0; i < horizontalChunk; ++i)
	{
		for(int j = 0; j < verticalChunk; ++j)
		{
			memmove(polyVolume + (horizontalSlice * i + horizontalSize * j + end), polyVolume + (horizontalSlice * i + horizontalSize * j), size);
		}
	}

	playerChunk.setX(playerChunk.getX() - 1);
}

void GraphicsManager::AdjustCamera(int xAxis, int yAxis)
{
	player->yaw(Ogre::Degree(xAxis));

	Ogre::Quaternion ori = c_sn->getOrientation();
	Ogre::Radian pitchRadian = ori.getPitch();
	Ogre::Real pitchDegree = pitchRadian.valueDegrees();

	Ogre::Real yAdjust = yAxis;
	if(yAdjust + pitchDegree >= 89.0 || pitchDegree + yAdjust <= -89.0)
	{

	}
	else
		c_sn->pitch(Ogre::Degree(yAxis));
}