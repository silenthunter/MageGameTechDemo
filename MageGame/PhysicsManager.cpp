//you need this. it creates pointer to characters which Havok Engine uses
//if you put this line into HavokUtilities.h you will get re-definition errors
//because all cpp files which includes HavokUtilities.h will try to create these character pointers in their .obj copies
//and this will create Linking redefinition problems
#include <Common/Base/keycode.cxx>

//You do not want to include the animation headers at this point
//if you comment this out it will give unresolved symbol errors for animation libraries
#ifdef HK_FEATURE_PRODUCT_ANIMATION
#undef HK_FEATURE_PRODUCT_ANIMATION
#endif
//I need to do that because of some memory linkings at compile time
//if you comment this out it will give unresolved symbol errors
#ifndef HK_EXCLUDE_LIBRARY_hkgpConvexDecomposition
#define HK_EXCLUDE_LIBRARY_hkgpConvexDecomposition
#endif

//this is for linking hkBase.lib
//if you comment this out it will give unresolved symbol errors for hkBase.lib
#include <Common/Base/Config/hkProductFeatures.cxx> 

//you want to include this for being able to define the following struct and function
#include <Physics/Collide/hkpCollide.h>	
//these lines are needed for getting rid of linker problems for hkpCollide.lib
//I could not find a better solution
//source for why I do this --> http://software.intel.com/en-us/forums/showthread.php?t=73886
struct hkTestEntry* hkUnitTestDatabase = HK_NULL; 
hkBool HK_CALL hkTestReport(hkBool32 cond, const char* desc, const char* file, int line) {return false;}

#include "PhysicsManager.h"


PhysicsManager::PhysicsManager(SimpleVolume<MaterialDensityPair44> *volume)
{
	chunkSize = WorldDataMap["ChunkSize"];
	initHavok();

	polyVolume = volume;
	int x = polyVolume->getWidth() / chunkSize;
	int y = polyVolume->getHeight() / chunkSize;
	int z = polyVolume->getDepth() / chunkSize;

	worldScale = 1.f;

	//Load the entire map
	UpdateChunkRange(Vector3DInt32(0, 0, 0), Vector3DInt32(x, y, z));
}


PhysicsManager::~PhysicsManager(void)
{
	deInitHavok();
}

void PhysicsManager::initHavok()
{
	{//initialize Havok Memory
		// Allocate 0.5MB of physics solver buffer.
		//hkMemoryRouter* m_pMemoryRouter;
		m_pMemoryRouter = hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(500000));
		hkBaseSystem::init( m_pMemoryRouter, errorReport );
	}
	{// Initialize the multi-threading classes, hkJobQueue, and hkJobThreadPool
		// Most of the comments are copied and pasted from ConsoleExampleMt.cpp of HavokDemos folder (2010 version)
		// They can be used for all Havok multithreading tasks. In this exmaple we only show how to use
		// them for physics, but you can reference other multithreading demos in the demo framework
		// to see how to multithread other products. The model of usage is the same as for physics.
		// The hkThreadpool has a specified number of threads that can run Havok jobs.  These can work
		// alongside the main thread to perform any Havok multi-threadable computations.
		// The model for running Havok tasks in Spus and in auxilary threads is identical.  It is encapsulated in the
		// class hkJobThreadPool.  On PlayStation(R)3 we initialize the SPU version of this class, which is simply a SPURS taskset.
		// On other multi-threaded platforms we initialize the CPU version of this class, hkCpuJobThreadPool, which creates a pool of threads
		// that run in exactly the same way.  On the PlayStation(R)3 we could also create a hkCpuJobThreadPool.  However, it is only
		// necessary (and advisable) to use one Havok PPU thread for maximum efficiency. In this case we simply use this main thread
		// for this purpose, and so do not create a hkCpuJobThreadPool.
		// Get the number of physical threads available on the system
		//hkHardwareInfo m_hardwareInfo;
		hkGetHardwareInfo(m_hardwareInfo);
		m_iTotalNumThreadsUsed = m_hardwareInfo.m_numThreads;

		// We use one less than this for our thread pool, because we must also use this thread for our simulation
		//hkCpuJobThreadPoolCinfo m_threadPoolCinfo;
		m_threadPoolCinfo.m_numThreads = m_iTotalNumThreadsUsed - 1;

		//RDS_PREVDEFINITIONS this line is from previous HavokWrapper
		//m_threadPoolCinfo.m_allocateRuntimeMemoryBlocks = true;
		// This line enables timers collection, by allocating 200 Kb per thread.  If you leave this at its default (0),
		// timer collection will not be enabled.
		m_threadPoolCinfo.m_timerBufferPerThreadAllocation = 200000;
		m_pThreadPool = new hkCpuJobThreadPool( m_threadPoolCinfo );

		// We also need to create a Job queue. This job queue will be used by all Havok modules to run multithreaded work.
		// Here we only use it for physics.
		m_jobQueuInfo.m_jobQueueHwSetup.m_numCpuThreads = m_iTotalNumThreadsUsed;
		m_pJobQueue = new hkJobQueue(m_jobQueuInfo);

		//
		// Enable monitors for this thread.
		//

		// Monitors have been enabled for thread pool threads already (see above comment).
		hkMonitorStream::getInstance().resize(200000);
	}

	{// <PHYSICS-ONLY>: Create the physics world.			
		// At this point you would initialize any other Havok modules you are using.
		// The world cinfo contains global simulation parameters, including gravity, solver settings etc.

		// Set the simulation type of the world to multi-threaded.
		m_worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;

		// Flag objects that fall "out of the world" to be automatically removed - just necessary for this physics scene
		// In other words, objects that fall "out of the world" will be automatically removed
		m_worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_REMOVE_ENTITY;
	
		//RDS_HARDCODED values here --> just for me to see if I can make this part better
		//standard gravity settings, collision tolerance and world size 
		m_worldInfo.m_gravity.set(0,-9.8f,0);
		//I do not know what the next line does. For this demo it doesnt change anything if/if not having this line enabled
		//m_worldInfo.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_8ITERS_MEDIUM);
		m_worldInfo.m_collisionTolerance = 0.1f;
		//This will effect the removal of objects when they fall out of the world.
		//If you have BROADPHASE_BORDER_REMOVE_ENTITY then your entities will be removed from Havok according to this number you set
		m_worldInfo.setBroadPhaseWorldSize(5000.0f);

		//initialize world with created info
		world = new hkpWorld(m_worldInfo);

		// RDS_PREVDEFINITIONS this line is from another HavokWrapper. Helps when using VisualDebugger
		// Disable deactivation, so that you can view timers in the VDB. This should not be done in your game.
		world->m_wantDeactivation = false;


		// When the simulation type is SIMULATION_TYPE_MULTITHREADED, in the debug build, the sdk performs checks
		// to make sure only one thread is modifying the world at once to prevent multithreaded bugs. Each thread
		// must call markForRead / markForWrite before it modifies the world to enable these checks.
		world->markForWrite();

		// Register all collision agents, even though only box - box will be used in this particular example.
		// It's important to register collision agents before adding any entities to the world.
		hkpAgentRegisterUtil::registerAllAgents( world->getCollisionDispatcher() );

		// We need to register all modules we will be running multi-threaded with the job queue
		world->registerWithJobQueue( m_pJobQueue );

		// Now we have finished modifying the world, release our write marker.
		world->unmarkForWrite();
	}

	{//RDS Begin --> you can use such a way to enable VisualDebugger
	 //so that you  do not need commenting out some lines all the time you change it
		#ifdef HAVOK_VISUAL_DEBUGGER_ENABLED
			registerVisualDebugger();
		#endif	
	}//RDS End
}

void PhysicsManager::registerVisualDebugger()
{
	// <PHYSICS-ONLY>: Register physics specific visual debugger processes
	// By default the VDB will show debug points and lines, however some products such as physics and cloth 
	// have additional viewers that can show geometries etc and can be enabled and disabled by the VDB app.
	{
		// The visual debugger so we can connect remotely to the simulation
		// The context must exist beyond the use of the VDB instance, and you can make
		// whatever contexts you like for your own viewer types.
	
		m_pPhysicsContext = new hkpPhysicsContext();// Create context for Visual Debugger
		hkpPhysicsContext::registerAllPhysicsProcesses(); //Reigster all the physics viewers
		
		world->markForWrite();//RDS for the following line to be able to have effect on m_pPhysicsWorld
		m_pPhysicsContext->addWorld(world); // add the physics world so the viewers can see it
		m_arrayPhysicsContext.pushBack(m_pPhysicsContext);

		// Now we have finished modifying the world, release our write marker.
		world->unmarkForWrite();
	}

	//Create VDB instance
	world->markForWrite();
	m_pVisualDebugger = new hkVisualDebugger(m_arrayPhysicsContext);
	m_pVisualDebugger->serve();
	world->unmarkForWrite();
}

void PhysicsManager::deInitHavok()
{
	//clean up physics world
	world->markForWrite();
	world->removeReference();

	//clean up visual debugger(if used)
	#ifdef HAVOK_VISUAL_DEBUGGER_ENABLED
		m_pVisualDebugger->removeReference();
		m_pPhysicsContext->removeReference();
	#endif	

	//delete job queue, thread poll, deallocate memory
	delete m_pJobQueue;
	m_pThreadPool->removeReference();

	//quit base system, this also deallocates the buffer which is used for Havok
	hkBaseSystem::quit();
}

void PhysicsManager::UpdateChunk(Vector3DInt32 &chunk)
{
	Vector3DInt32 start(chunk.getX() * chunkSize, chunk.getY() * chunkSize, chunk.getZ() * chunkSize);
	Vector3DInt32 end = start + Vector3DInt32(chunkSize, chunkSize, chunkSize);

	SurfaceMesh<PositionMaterialNormal> mesh;
	CubicSurfaceExtractorWithNormals<SimpleVolume, MaterialDensityPair44> surfaceExtractor(polyVolume, Region(start, end), &mesh);
	surfaceExtractor.execute();

	std::vector<uint32_t> vecIndices = mesh.getIndices();
	std::vector<PositionMaterialNormal> vecVertices = mesh.getVertices();

	if(vecVertices.size() == 0) return; //No vertices to add to the world

	hkpExtendedMeshShape::TrianglesSubpart subPart;
	float* vert;
	unsigned long *ind;
	hkpExtendedMeshShape *shp;

	shp = new hkpExtendedMeshShape();
	subPart.m_numVertices = vecVertices.size() * 3;
	subPart.m_numTriangleShapes = vecIndices.size() / 3;
	subPart.m_vertexStriding = sizeof(float) * 3;
	subPart.m_indexStriding = sizeof(unsigned long) * 3;
	vert = new float[subPart.m_numVertices];
	ind = new unsigned long[subPart.m_numTriangleShapes * 3];

	//Get vertices
	std::vector<PositionMaterialNormal>::iterator vecItr;
	int vecCnt = 0;
	for(vecItr = vecVertices.begin(); vecItr != vecVertices.end(); vecItr++, vecCnt++)
	{
		PolyVox::Vector3DFloat pos = (*vecItr).getPosition();
		pos += Vector3DFloat(chunk.getX() * chunkSize, chunk.getY() * chunkSize, chunk.getZ() * chunkSize);
		pos *= worldScale;

		vert[vecCnt * 3 + 0] = pos.getX();
		vert[vecCnt * 3 + 1] = pos.getY();
		vert[vecCnt * 3 + 2] = pos.getZ();

	}

	//Get indices
	std::vector<uint32_t>::iterator indVec;
	int indCnt = 0;
	for(indVec = vecIndices.begin(); indVec != vecIndices.end(); indVec++, indCnt++)
	{
		ind[indCnt] = *indVec;
	}

	subPart.m_vertexBase = vert;
	subPart.m_indexBase = ind;
	subPart.m_stridingType = hkpExtendedMeshShape::INDICES_INT32;
	shp->addTrianglesSubpart(subPart);

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

void PhysicsManager::UpdateChunkRange(Vector3DInt32 &start, Vector3DInt32 &end)
{
	if(start.getX() > end.getX() || start.getY() > end.getY() || start.getZ() > end.getZ()) return; //start needs to be lower than end

	for(int i = start.getX(); i <= end.getX(); i++)
		for(int j = start.getY(); j <= end.getY(); j++)
			for(int k = start.getZ(); k <= end.getZ(); k++)
				UpdateChunk(Vector3DInt32(i, j, k));
}

void PhysicsManager::StepSimulation(float deltaTime)
{
	if(deltaTime <= 0) return; //You can't simulate nothing!

	//step multithreaded simulation using this thread and all threads in the thread pool
	world->stepMultithreaded(m_pJobQueue, m_pThreadPool, deltaTime);
	stepVisualDebugger(deltaTime);
	hkMonitorStream::getInstance().reset();
	m_pThreadPool->clearTimerData();	
}

void PhysicsManager::stepVisualDebugger(float deltaTime)
{
	#ifdef HAVOK_VISUAL_DEBUGGER_ENABLED
		//synchronize the timer data and step Visual Debugger
		m_pPhysicsContext->syncTimers( m_pThreadPool );
		m_pVisualDebugger->step();
	#endif	
}
