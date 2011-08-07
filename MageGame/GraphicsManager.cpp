#include "GraphicsManager.h"

#define NUM_TEX_PER_ROW 6
#define NUM_TEX_PER_COL 17
#define NUM_NONTEX_MATERIALS 1
#define TEX_WIDTH_NORMALIZED (1.0 / NUM_TEX_PER_ROW)
#define TEX_HEIGHT_NORMALIZED (1.0 / NUM_TEX_PER_COL)

using std::string;
using std::vector;

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

GraphicsManager::GraphicsManager(int gm_chunkSize,
					float gm_worldScale,
					float gm_blockItemWorldScale,
					int gm_viewDist,
					int gm_verticalMax,
					int gm_verticalChunk,
					PolyVox::SimpleVolume<VoxelMat> *volData,
					WorldTerrain *wTerra,
					GameParser *gm_gameParser)
					:
					chunkSize(gm_chunkSize),
					worldScale(gm_worldScale),
					blockItemHalfScale(gm_blockItemWorldScale / 2.0f),
					centerChunk(gm_viewDist),
					verticalMax(gm_verticalMax),
					verticalChunk(gm_verticalMax / chunkSize),
					polyVolume(volData),
					wTer(wTerra),
					gameParser(gm_gameParser)
{
	init();

	playerChunk.setX(centerChunk);
	playerChunk.setZ(centerChunk);

	oneSixth = 1.0f / 6;
	twoSixth = 2.0f / 6;
	threeSixth = 3.0f / 6;
	fourSixth = 4.0f / 6;
	fiveSixth = 5.0f / 6;

	int horizontalChunk = gm_viewDist * 2 + 1;
	for(int i = 0; i < horizontalChunk; ++i)
	{
		for(int j = 0; j < horizontalChunk; ++j)
		{
			InitVoxels(i, j, 0, 0);
			LoadMO(i, j, 0, 0);
		}
	}
}

GraphicsManager::~GraphicsManager(void)
{
	delete root;
	manualObjects.clear();
	itemBlocks.clear();
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

void GraphicsManager::InitVoxels(int chunkNumX, int chunkNumZ, int xDiff, int zDiff)
{
    float regionWidth = 1.0;
    float regionDepth = 8.0;
    float regionHeight = 1.0;

    float chunkX = (chunkNumX + xDiff) * regionWidth;
    float chunkZ = (chunkNumZ + zDiff) * regionHeight;

    int xOffset = chunkNumX * chunkSize;
    int zOffset = chunkNumZ * chunkSize;

	for(int x = 0; x < chunkSize; ++x)
	{
		for(int y = 0; y < verticalMax; ++y)
		{
			for(int z = 0; z < chunkSize; ++z)
			{
				double nx = (double)x / chunkSize;
				double ny = (double)y / verticalMax;
				double nz = (double)z / chunkSize;

				nx = chunkX + nx * regionWidth;
				ny *= regionDepth;
				nz = chunkZ + nz * regionHeight;

				double v = wTer->worldTerrain.get(nx, ny, nz);

				if(v > 0)
				{
					VoxelMat vox;
					vox.setMaterial(v);
					polyVolume->setVoxelAt(x + xOffset, y, z + zOffset, vox);
				}
			}
		}
	}
}

void GraphicsManager::LoadMO(int chunkNumX, int chunkNumZ, int xDiff, int zDiff)
{
	unsigned int xChunkSize = chunkNumX * chunkSize;
	unsigned int zChunkSize = chunkNumZ * chunkSize;
	unsigned int chunkAddition = chunkSize - 1;

	for(int i = 0; i < verticalChunk; ++i)
	{
		int yChunkSize = i * chunkSize;
		Vector3DInt32 start(xChunkSize, yChunkSize, zChunkSize);
		Vector3DInt32 end(xChunkSize + chunkAddition, yChunkSize + chunkAddition, zChunkSize + chunkAddition);

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
		sprintf(str, "%d_%d_%d", chunkNumX + xDiff, i, chunkNumZ + zDiff);

		//Set the object with the render operation and the .material
		obj->begin("VoxelTexture", Ogre::RenderOperation::OT_TRIANGLE_LIST);

		//Work with the vertices
		Vector3DFloat posOffset = Vector3DFloat(xChunkSize + xDiff * chunkSize, yChunkSize, zChunkSize + zDiff * chunkSize) * worldScale;
		for(vector<PositionMaterial>::const_iterator vecItr = vecVertices.begin(); vecItr != vecVertices.end(); vecItr++)
		{
			Vector3DFloat pos = vecItr->getPosition() * worldScale;
			pos += posOffset;
			obj->position(pos.getX(), pos.getY(), pos.getZ());
			Ogre::ColourValue val;
			VoxelMat vMat = vecItr->getMaterial();
			uint16_t mat = vMat.getMaterial() - NUM_NONTEX_MATERIALS;
			/*
			r = v coordinate of where the texture starts
			g = direction of the block with 0.1, 0.2, 0.3, and 0.4 as North, East, South, and West
			b = ?
			a = alpha?
			*/
			val.r = mat * TEX_HEIGHT_NORMALIZED;
			val.g = 0.0f;
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
}

void GraphicsManager::DestroyMO(int chunkNumX, int chunkNumZ, int xDiff, int zDiff)
{
	for(int i = 0; i < verticalChunk; ++i)
	{
		char str[50];
		sprintf(str, "%d_%d_%d", chunkNumX + xDiff, i, chunkNumZ + zDiff);
		manualObjects.erase(str);
	}
}

VoxelMat GraphicsManager::RemoveBlock(PolyVox::Vector3DInt32 &chunk, PolyVox::Vector3DInt32 blockPos)
{
	VoxelMat vox = polyVolume->getVoxelAt(blockPos);
	polyVolume->setVoxelAt(blockPos, 0);

	int xDiff = playerChunk.getX() - centerChunk;
	int zDiff = playerChunk.getZ() - centerChunk;

	UpdateChunk(chunk, xDiff, zDiff);
	
	int posX = blockPos.getX();
	int posY = blockPos.getY();
	int posZ = blockPos.getZ();
	int chunkX = chunk.getX();
	int chunkY = chunk.getY();
	int chunkZ = chunk.getZ();
	unsigned short chunkSizeEnd = chunkSize - 1;

	if(posX != 0 && posX % chunkSize == 0)
	{
		UpdateChunk(Vector3DInt32(chunkX - 1, chunkY, chunkZ), xDiff, zDiff);
	}
	else if(posX != wTer->currWidth - 1 && posX % chunkSize == chunkSizeEnd)
	{
		UpdateChunk(Vector3DInt32(chunkX + 1, chunkY, chunkZ), xDiff, zDiff);
	}

	if(posY != 0 && posY % chunkSize == 0)
	{
		UpdateChunk(Vector3DInt32(chunkX, chunkY - 1, chunkZ), xDiff, zDiff);
	}
	else if(posY != wTer->currDepth - 1 && posY % chunkSize == chunkSizeEnd)
	{
		UpdateChunk(Vector3DInt32(chunkX, chunkY + 1, chunkZ), xDiff, zDiff);
	}

	if(posZ != 0 && posZ % chunkSize == 0)
	{
		UpdateChunk(Vector3DInt32(chunkX, chunkY, chunkZ - 1), xDiff, zDiff);
	}
	else if(posZ != wTer->currHeight - 1 && posZ % chunkSize == chunkSizeEnd)
	{
		UpdateChunk(Vector3DInt32(chunkX, chunkY, chunkZ + 1), xDiff, zDiff);
	}

	return vox;
}

void GraphicsManager::UpdateChunk(Vector3DInt32 chunkNum, int xDiff, int zDiff)
{
	int i = chunkNum.getX();
	int j = chunkNum.getY();
	int k = chunkNum.getZ();

	char str[50];
	sprintf(str, "%d_%d_%d", i + xDiff, j, k + zDiff);

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

	Vector3DFloat posOffset = Vector3DFloat(iChunkSize + xDiff * chunkSize, jChunkSize, kChunkSize + zDiff * chunkSize) * worldScale;
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

	int horizontalChunk = centerChunk * 2 - 1;
	int horizontalSize = horizontalChunk * chunkSize;
	int size = horizontalSize * verticalMax;
	int lastChunk = horizontalChunk - 1;

	//Save the chunks you won't see anymore
	for(int i = 0; i < horizontalChunk; ++i)
	{
		gameParser->StoreChunks(i, lastChunk, xDiff, zDiff);
		DestroyMO(i, lastChunk, xDiff, zDiff);
	}

	//Move the voxels
	memmove(polyVolume + size, polyVolume, size);

	//Recalculate playerChunk
	playerChunk.setZ(playerChunk.getZ() - 1);
	//Recalculate xDiff and zDiff
	xDiff = playerChunk.getX() - centerChunk;
	zDiff = playerChunk.getZ() - centerChunk;

	//Attempt to load the chunks you'll need
	for(int i = 0; i < horizontalChunk; ++i)
	{
		if(!gameParser->LoadChunks(i, 0, xDiff, zDiff))
		{
			InitVoxels(i, 0, xDiff, zDiff); //Chunk hasn't been loaded yet, generate it
		}
		LoadMO(i, 0, xDiff, zDiff);
	}
}

void GraphicsManager::MoveSouth()
{
	int xDiff = playerChunk.getX() - centerChunk;
	int zDiff = playerChunk.getZ() - centerChunk;

	int horizontalChunk = centerChunk * 2 - 1;
	int horizontalSize = horizontalChunk * chunkSize;
	int start = horizontalSize * verticalMax;
	int size = (horizontalSize - 1) * horizontalSize * verticalMax;
	int lastChunk = horizontalChunk - 1;

	//Save the chunks you won't see anymore
	for(int i = 0; i < horizontalChunk; ++i)
	{
		gameParser->StoreChunks(i, 0, xDiff, zDiff);
		DestroyMO(i, 0, xDiff, zDiff);
	}

	//Move the voxels
	memmove(polyVolume, polyVolume + start, size);

	//Recalculate playerChunk
	playerChunk.setZ(playerChunk.getZ() + 1);
	//Recalculate xDiff and zDiff
	xDiff = playerChunk.getX() - centerChunk;
	zDiff = playerChunk.getZ() - centerChunk;

	//Attempt to load the chunks you'll need
	for(int i = 0; i < horizontalChunk; ++i)
	{
		if(!gameParser->LoadChunks(i, lastChunk, xDiff, zDiff))
		{
			InitVoxels(i, lastChunk, xDiff, zDiff); //Chunk hasn't been loaded yet, generate it
		}
		LoadMO(i, lastChunk, xDiff, zDiff);
	}
}

void GraphicsManager::MoveEast()
{
	int xDiff = playerChunk.getX() - centerChunk;
	int zDiff = playerChunk.getZ() - centerChunk;

	int horizontalChunk = centerChunk * 2 - 1;
	int horizontalSize = horizontalChunk * chunkSize;
	int horizontalSlice = horizontalSize * verticalMax;
	int start = chunkSize;
	int size = horizontalSize - chunkSize;
	int lastChunk = horizontalChunk - 1;

	//Save the chunks you won't see anymore
	for(int i = 0; i < horizontalChunk; ++i)
	{
		gameParser->StoreChunks(0, i, xDiff, zDiff);
		DestroyMO(0, i, xDiff, zDiff);
	}

	//Move the voxels
	for(int i = 0; i < horizontalSize; ++i)
	{
		for(int j = 0; j < verticalMax; ++j)
		{
			memmove(polyVolume + (horizontalSlice * i + horizontalSize * j), polyVolume + (horizontalSlice * i + horizontalSize * j + chunkSize), size);
		}
	}

	//Recalculate playerChunk
	playerChunk.setX(playerChunk.getX() + 1);
	//Recalculate xDiff and zDiff
	xDiff = playerChunk.getX() - centerChunk;
	zDiff = playerChunk.getZ() - centerChunk;

	//Attempt to load the chunks you'll need
	for(int i = 0; i < horizontalChunk; ++i)
	{
		if(!gameParser->LoadChunks(lastChunk, i, xDiff, zDiff))
		{
			InitVoxels(lastChunk, i, xDiff, zDiff); //Chunk hasn't been loaded yet, generate it
		}
		LoadMO(lastChunk, i, xDiff, zDiff);
	}
}

void GraphicsManager::MoveWest()
{
	int xDiff = playerChunk.getX() - centerChunk;
	int zDiff = playerChunk.getZ() - centerChunk;

	int horizontalChunk = centerChunk * 2 - 1;
	int horizontalSize = horizontalChunk * chunkSize;
	int horizontalSlice = horizontalSize * verticalMax;
	int size = horizontalSize - chunkSize;
	int lastChunk = horizontalChunk - 1;

	//Save the chunks you won't see anymore
	for(int i = 0; i < horizontalChunk; ++i)
	{
		gameParser->StoreChunks(lastChunk, i, xDiff, zDiff);
		DestroyMO(lastChunk, i, xDiff, zDiff);
	}

	//Move the voxels
	for(int i = 0; i < horizontalSize; ++i)
	{
		for(int j = 0; j < verticalMax; ++j)
		{
			cout << "Dest: " << (horizontalSlice * i + horizontalSize * j + chunkSize) << endl;
			cout << "Start: " << (horizontalSlice * i + horizontalSize * j) << endl;
			cout << "Size: "<< size << endl;
			cout << "i: " << i << endl;
			cout << "j: " << j << endl;
			memmove(polyVolume + (horizontalSlice * i + horizontalSize * j + chunkSize), polyVolume + (horizontalSlice * i + horizontalSize * j), size);
		}
	}

	//Recalculate playerChunk
	playerChunk.setX(playerChunk.getX() - 1);
	//Recalculate xDiff and zDiff
	xDiff = playerChunk.getX() - centerChunk;
	zDiff = playerChunk.getZ() - centerChunk;

	//Attempt to load the chunks you'll need
	for(int i = 0; i < horizontalChunk; ++i)
	{
		if(!gameParser->LoadChunks(0, i, xDiff, zDiff))
		{
			InitVoxels(0, i, xDiff, zDiff); //Chunk hasn't been loaded yet, generate it
		}
		LoadMO(0, i, xDiff, zDiff);
	}
}

void GraphicsManager::AddItemBlock(PolyVox::Vector3DInt32 blockPos, VoxelMat cubeMat, double time)
{
	float lowerPosX = -blockItemHalfScale;
	float upperPosX = blockItemHalfScale;
	float lowerPosY = -blockItemHalfScale;
	float upperPosY = blockItemHalfScale;
	float lowerPosZ = -blockItemHalfScale;
	float upperPosZ = blockItemHalfScale;
	
	//Set the coordinates for the right texture in the texture atlas
	uint16_t mat = cubeMat.getMaterial() - NUM_NONTEX_MATERIALS;
	float vCoordStart = mat * TEX_HEIGHT_NORMALIZED;
	float vCoordEnd = vCoordStart + TEX_HEIGHT_NORMALIZED;

	ManualObject *cubeMO = manager->createManualObject();

	///Bottom Face
	cubeMO->begin("ItemBlockTexture", Ogre::RenderOperation::OT_TRIANGLE_FAN);
	cubeMO->position(lowerPosX, lowerPosY, lowerPosZ); //0
	cubeMO->normal(0, -1, 0);
	cubeMO->textureCoord(oneSixth, vCoordEnd);
	cubeMO->position(upperPosX, lowerPosY, lowerPosZ); //1
	cubeMO->normal(0, -1, 0);
	cubeMO->textureCoord(twoSixth, vCoordEnd);
	cubeMO->position(upperPosX, lowerPosY, upperPosZ); //2
	cubeMO->normal(0, -1, 0);
	cubeMO->textureCoord(twoSixth, vCoordStart);
	cubeMO->position(lowerPosX, lowerPosY, upperPosZ); //3
	cubeMO->normal(0, -1, 0);
	cubeMO->textureCoord(oneSixth, vCoordStart);
	cubeMO->end();

	//Left face
	cubeMO->begin("ItemBlockTexture", Ogre::RenderOperation::OT_TRIANGLE_FAN);
	cubeMO->position(lowerPosX, lowerPosY, upperPosZ); //3
	cubeMO->normal(-1, 0, 0);
	cubeMO->textureCoord(1, vCoordEnd);
	cubeMO->position(lowerPosX, upperPosY, upperPosZ); //7
	cubeMO->normal(-1, 0, 0);
	cubeMO->textureCoord(1, vCoordStart);
	cubeMO->position(lowerPosX, upperPosY, lowerPosZ); //4
	cubeMO->normal(-1, 0, 0);
	cubeMO->textureCoord(fiveSixth, vCoordStart);
	cubeMO->position(lowerPosX, lowerPosY, lowerPosZ); //0
	cubeMO->normal(-1, 0, 0);
	cubeMO->textureCoord(fiveSixth, vCoordEnd);
	cubeMO->end();

	//Top face
	cubeMO->begin("ItemBlockTexture", Ogre::RenderOperation::OT_TRIANGLE_FAN);
	cubeMO->position(lowerPosX, upperPosY, lowerPosZ); //4
	cubeMO->normal(0, 1, 0);
	cubeMO->textureCoord(0, vCoordStart);
	cubeMO->position(lowerPosX, upperPosY, upperPosZ); //7
	cubeMO->normal(0, 1, 0);
	cubeMO->textureCoord(0, vCoordEnd);
	cubeMO->position(upperPosX, upperPosY, upperPosZ); //6
	cubeMO->normal(0, 1, 0);
	cubeMO->textureCoord(oneSixth, vCoordEnd);
	cubeMO->position(upperPosX, upperPosY, lowerPosZ); //5
	cubeMO->normal(0, 1, 0);
	cubeMO->textureCoord(oneSixth, vCoordStart);
	cubeMO->end();
	
	//Right face
	cubeMO->begin("ItemBlockTexture", Ogre::RenderOperation::OT_TRIANGLE_FAN);
	cubeMO->position(upperPosX, lowerPosY, lowerPosZ); //1
	cubeMO->normal(1, 0, 0);
	cubeMO->textureCoord(fourSixth, vCoordEnd);
	cubeMO->position(upperPosX, upperPosY, lowerPosZ); //5
	cubeMO->normal(1, 0, 0);
	cubeMO->textureCoord(fourSixth, vCoordStart);
	cubeMO->position(upperPosX, upperPosY, upperPosZ); //6
	cubeMO->normal(1, 0, 0);
	cubeMO->textureCoord(threeSixth, vCoordStart);
	cubeMO->position(upperPosX, lowerPosY, upperPosZ); //2
	cubeMO->normal(1, 0, 0);
	cubeMO->textureCoord(threeSixth, vCoordEnd);
	cubeMO->end();
 
	//Back face
	cubeMO->begin("ItemBlockTexture", Ogre::RenderOperation::OT_TRIANGLE_FAN);
	cubeMO->position(upperPosX, lowerPosY, lowerPosZ); //1
	cubeMO->normal(0, 0, -1);
	cubeMO->textureCoord(fourSixth, vCoordEnd);
	cubeMO->position(lowerPosX, lowerPosY, lowerPosZ); //0
	cubeMO->normal(0, 0, -1);
	cubeMO->textureCoord(fiveSixth, vCoordEnd);
	cubeMO->position(lowerPosX, upperPosY, lowerPosZ); //4
	cubeMO->normal(0, 0, -1);
	cubeMO->textureCoord(fiveSixth, vCoordStart);
	cubeMO->position(upperPosX, upperPosY, lowerPosZ); //5
	cubeMO->normal(0, 0, -1);
	cubeMO->textureCoord(fourSixth, vCoordStart);
	cubeMO->end();
 
	//Front face
	cubeMO->begin("ItemBlockTexture", Ogre::RenderOperation::OT_TRIANGLE_FAN);
	cubeMO->position(upperPosX, lowerPosY, upperPosZ); //2
	cubeMO->normal(0, 0, 1);
	cubeMO->textureCoord(threeSixth, vCoordEnd);
	cubeMO->position(upperPosX, upperPosY, upperPosZ); //6
	cubeMO->normal(0, 0, 1);
	cubeMO->textureCoord(threeSixth, vCoordStart);
	cubeMO->position(lowerPosX, upperPosY, upperPosZ); //7
	cubeMO->normal(0, 0, 1);
	cubeMO->textureCoord(twoSixth, vCoordStart);
	cubeMO->position(lowerPosX, lowerPosY, upperPosZ); //3
	cubeMO->normal(0, 0, 1);
	cubeMO->textureCoord(twoSixth, vCoordEnd);
	cubeMO->end();

	Ogre::SceneNode *cubeSN = manager->createSceneNode();
	cubeSN->setPosition(blockPos.getX(), blockPos.getY(), blockPos.getZ());
	cubeSN->attachObject(cubeMO);
	root_sn->addChild(cubeSN); //Attach the new sceneNode to the root scenenode
	
	ItemBlock cubeBlock = {cubeSN, time};
	itemBlocks.push_back(cubeBlock); //Save it in the vector
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

std::vector<ItemBlock> GraphicsManager::getItemBlocks()
{
	return itemBlocks;
}
