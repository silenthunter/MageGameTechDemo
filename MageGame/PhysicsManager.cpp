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

using PolyVox::SimpleVolume;
using PolyVox::Region;
using PolyVox::PositionMaterial;
using PolyVox::CubicSurfaceExtractor;
using PolyVox::Vector3DInt32;
using PolyVox::Vector3DFloat;
using PolyVox::SurfaceMesh;


PhysicsManager::PhysicsManager(PolyVox::SimpleVolume<VoxelMat>* volume, GraphicsManager *manager)
{
	graphMan = manager;
	chunkSize = WorldDataMap["ChunkSize"];
	initHavok();
	initPlayer();

	polyVolume = volume;
	int x = polyVolume->getWidth() / chunkSize;
	int y = polyVolume->getHeight() / chunkSize;
	int z = polyVolume->getDepth() / chunkSize;

	worldScale = 1.f;
	maxPageSize = 45;
	maxRender = 45;

	//Load the entire map
	//UpdateChunkRange(Vector3DInt32(0, 0, 0), Vector3DInt32(x, y, z));
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

		m_breakUtil = new hkpBreakOffPartsUtil(this);
		world->addWorldExtension(m_breakUtil);

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

static int threadNum = 0;

void PhysicsManager::UpdateChunk(Vector3DInt32 &chunk)
{
	if(threadNum > 5) return;//TOO MANY THREADS

	int upperX = polyVolume->getWidth() / chunkSize;
	int upperY = polyVolume->getHeight() / chunkSize;
	int upperZ = polyVolume->getDepth() / chunkSize;

	if(chunk.getX() < 0 || chunk.getY() < 0 || chunk.getZ() < 0) return;//Lower bounds check
	if(chunk.getX() > upperX || chunk.getY() > upperY || chunk.getZ() > upperZ) return;//Upper bounds check

	mu.lock();
	if(pageQueue.getSize() >= maxPageSize)//Need to delete the LRU chunk
	{
		Vector3DInt32 LRUchunk = pageQueue[0];
		pageQueue.removeAtAndCopy(0);//Pop the front chunk
		hkpRigidBody* body = physicsMap[LRUchunk];
		physicsMap.erase(LRUchunk);//delete the rigid body from the physics map
		
		if(body && body->getWorld())//Ignore NULL values
		{
			world->lock();
			world->removeEntity(body);
			world->unlock();
		}
	}

	bool isPaged = false;
	int queuePos = -1;

	for(int i = 0; i < pageQueue.getSize(); i++)
		if(pageQueue[i] == chunk)
		{
			isPaged = true;
			pageQueue.removeAtAndCopy(i);//Remove the element and shift the array
			queuePos = i;
			break;
		}
	pageQueue.pushBack(chunk);//put chunk at the back of the queue

	//The chunk is still in memory
	if(isPaged)
	{
		if(queuePos >= maxRender)//This chunk currently isn't in the world
		{
			hkpRigidBody *body = physicsMap[chunk];
			world->lock();
			world->addEntity(body);
			world->unlock();
		}
		mu.unlock();
		return;
	}
	mu.unlock();

	Vector3DInt32* chunkPtr = new Vector3DInt32(chunk.getX(), chunk.getY(), chunk.getZ());
	boost::thread t(&PhysicsManager::Thread_UpdateChunk, this, chunkPtr);
	t.detach();
}

void PhysicsManager::Thread_UpdateChunk(Vector3DInt32* chunk)
{
	mu.lock();
	threadNum++;
	mu.unlock();

	//Init the thread's heap
	hkMemoryRouter memoryRouter;
	hkMemorySystem::getInstance().threadInit( memoryRouter, "hkCpuJobThreadPool" );
	hkBaseSystem::initThread( &memoryRouter );
	{

	hkpRigidBodyCinfo info;
	hkArray<hkpConvexShape*> shapes;
	hkArray<hkpShapeKey> keys;
	hkpBoxShape* box = new hkpBoxShape(hkVector4(worldScale / 2, worldScale / 2, worldScale / 2));

	int keyNum = 0;

	int chunkX = (chunk->getX() + 1) * chunkSize;
	int chunkY = (chunk->getY() + 1) * chunkSize;
	int chunkZ = (chunk->getZ() + 1) * chunkSize;

	for(int i = chunk->getX() * chunkSize; i < chunkX; i++)
	{
		for(int j = chunk->getY() * chunkSize; j < chunkY; j++)
		{
			for(int k = chunk->getZ() * chunkSize; k < chunkZ; k++)
			{
				VoxelMat voxel = polyVolume->getVoxelAt(PolyVox::Vector3DInt32(i, j, k));

				if(voxel.getDensity() > 0 && shapes.getSize() < 32760)
				{
					hkTransform tran;
					tran.setTranslation(hkVector4(worldScale * i, worldScale * j, worldScale * k));
					tran.setRotation(hkQuaternion::getIdentity());
					hkpConvexTransformShape* trans = new hkpConvexTransformShape(box, tran);

					trans->setUserData((hkUlong)(new PolyVox::Vector3DInt32(i, j, k)));

					shapes.pushBack(trans);
				}
			}
		}
	}

	if(shapes.getSize() > 0)
	{

		hkpExtendedMeshShape* chunkShape = new hkpExtendedMeshShape();
		hkpExtendedMeshShape::ShapesSubpart sub(&shapes[0], shapes.getSize());

		chunkShape->addShapesSubpart(sub);
		int num = chunkShape->getNumChildShapes();
		std::map<PolyVox::Vector3DInt32, hkpShapeKey> *keyMap = new std::map<PolyVox::Vector3DInt32, hkpShapeKey>();

		hkpShapeKey shpKey = chunkShape->getFirstKey();
		while(shpKey != HK_INVALID_SHAPE_KEY)
		{
			keys.pushBack(shpKey);
			hkpShapeBuffer buff;
			PolyVox::Vector3DInt32* vec = (PolyVox::Vector3DInt32*)chunkShape->getChildShape(shpKey, buff)->getUserData();
			(*keyMap)[*vec] = shpKey;

			shpKey = chunkShape->getNextKey(shpKey);
		}

		//hkpListShape *chunkShape = new hkpListShape(&shapes[0], shapes.getSize());

		hkpMoppCompilerInput mci;
		mci.m_enableChunkSubdivision = true;

		hkpMoppCode *code = hkpMoppUtility::buildCode(chunkShape, mci);

		hkpMoppBvTreeShape* moppShape = new hkpMoppBvTreeShape(chunkShape, code);

		info.m_motionType = hkpMotion::MOTION_FIXED;
		info.m_shape = moppShape;
		info.m_position = hkVector4(0, 0, 0);
		info.m_numShapeKeysInContactPointProperties = -1;

		hkpRigidBody *body = new hkpRigidBody(info);

		mu.lock();
		physicsMap[*chunk] = body;
		mu.unlock();
	
		body->setUserData((hkUlong)keyMap);

		for(int i = 0; i < keys.getSize(); i++)
			m_breakUtil->markPieceBreakable(body, keys[i], 100.f);

		//hkpBreakOffPartsUtil::removeKeysFromListShape(body, &keys[0], keys.getSize());

		/*world->lock();
		world->markForWrite();
		world->addEntity(body);
		world->unmarkForWrite();
		body->removeReference();*/
		chunkShape->removeReference();
		box->removeReference();

		mu.lock();
		readyRigidBodies.enqueue(body);
		mu.unlock();

		//world->unlock();
	}
	}

	hkBaseSystem::quitThread();
	hkMemorySystem::getInstance().threadQuit( memoryRouter );

	mu.lock();
	threadNum--;
	mu.unlock();

	return;
}

void PhysicsManager::UpdateChunkRange(Vector3DInt32 &start, Vector3DInt32 &end)
{
	if(start.getX() > end.getX() || start.getY() > end.getY() || start.getZ() > end.getZ()) return; //start needs to be lower than end

	GameTimer timer;
	double totalElapsed = 0.f;

	for(int i = start.getX(); i <= end.getX(); i++)
		for(int j = start.getY(); j <= end.getY(); j++)
			for(int k = start.getZ(); k <= end.getZ(); k++)
			{
				totalElapsed += timer.getElapsedTimeSec();
				if(totalElapsed >= .5f)
				{
					totalElapsed = 0;
					float progress = (float)(i * end.getY() + j) / (float)(end.getX() * end.getY());
					graphMan->UpdatePhysicsProgress(progress);
				}

				UpdateChunk(Vector3DInt32(i, j, k));
			}
}

void PhysicsManager::StepSimulation(float deltaTime)
{
	if(deltaTime <= 0) return; //You can't simulate nothing!

	//Load next available chunk
	if(!readyRigidBodies.isEmpty())
	{
		hkpRigidBody* nextRigid = NULL;
		readyRigidBodies.dequeue(nextRigid);

		world->markForWrite();
		world->lock();

		world->addEntity(nextRigid);
		nextRigid->removeReference();

		world->unlock();
		world->unmarkForWrite();
	}

	//Make sure the chunks surrounding the player are loaded
	hkVector4 playerPos = m_characterRigidBody->getPosition();
	Vector3DInt32 chunkNum(playerPos(0) / chunkSize, playerPos(1) / chunkSize, playerPos(2) / chunkSize);

	Vector3DInt32 start = chunkNum - Vector3DInt32(1, 1, 1);
	Vector3DInt32 end = chunkNum + Vector3DInt32(1, 1, 1);

	UpdateChunkRange(start, end);

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

void PhysicsManager::initPlayer()
{
	hkVector4 vertexA(0, 0.4f, 0);
	hkVector4 vertexB(0, -0.4f, 0);

	// Create a capsule to represent the character standing
	hkpCapsuleShape* m_standShape = new hkpCapsuleShape(vertexA, vertexB, .4f);

	// Create a capsule to represent the character crouching
	// Note that we create the smaller capsule with the base at the same position as the larger capsule.		
	vertexA.setZero4();


	hkpCharacterRigidBodyCinfo info;
	
	info.m_mass = 10.0f;
	info.m_maxForce = 1000.0f;
	info.m_up = hkVector4(0, 1.0f, 0);
	info.m_shape = m_standShape;
	info.m_position = hkVector4(50, 300, 50);

	info.m_maxSlope = 70.0f * HK_REAL_DEG_TO_RAD;

	m_characterRigidBody = new hkpCharacterRigidBody( info );
	{
		hkpCharacterRigidBodyListener* listener = new hkpCharacterRigidBodyListener();
		m_characterRigidBody->setListener( listener );
		listener->removeReference();
	}

	// Create the Character state machine and context
	{
		hkpCharacterState* state;
		hkpCharacterStateManager* manager = new hkpCharacterStateManager();

		state = new hkpCharacterStateOnGround();
		manager->registerState( state,	HK_CHARACTER_ON_GROUND);
		state->removeReference();

		state = new hkpCharacterStateInAir();
		manager->registerState( state,	HK_CHARACTER_IN_AIR);
		state->removeReference();

		state = new hkpCharacterStateJumping();
		((hkpCharacterStateJumping*)state)->setJumpHeight(20);
		manager->registerState( state,	HK_CHARACTER_JUMPING);
		state->removeReference();

		state = new hkpCharacterStateClimbing();
		manager->registerState( state,	HK_CHARACTER_CLIMBING);
		state->removeReference();

		m_characterContext = new hkpCharacterContext( manager, HK_CHARACTER_ON_GROUND );
		manager->removeReference();

		// Set character type
		m_characterContext->setCharacterType(hkpCharacterContext::HK_CHARACTER_RIGIDBODY);
	}

	world->lock();
	world->addEntity( m_characterRigidBody->getRigidBody() );
	world->unlock();
}

void PhysicsManager::UpdatePlayer(OIS::Keyboard* keyboard, OIS::Mouse* mouse, hkQuaternion &orientation, float m_timestep)
{
	hkpCharacterInput input;
	hkpCharacterOutput output;
	{
		input.m_inputLR = 0;
		input.m_inputUD = 0;

		if(keyboard->isKeyDown(OIS::KC_W)) input.m_inputUD += 1;
		if(keyboard->isKeyDown(OIS::KC_S)) input.m_inputUD -= 1;
		if(keyboard->isKeyDown(OIS::KC_A)) input.m_inputLR -= 1;
		if(keyboard->isKeyDown(OIS::KC_D)) input.m_inputLR += 1;

		input.m_wantJump =  keyboard->isKeyDown(OIS::KC_SPACE);
		input.m_atLadder = false;//atLadder;

		hkVector4 UP = hkVector4(0, 1.f, 0);
		input.m_up = UP;
		input.m_forward.set(0,0,1.f);
		input.m_forward.setRotatedDir(orientation, input.m_forward );//m_currentOrient

		hkStepInfo stepInfo;
		stepInfo.m_deltaTime = m_timestep;
		stepInfo.m_invDeltaTime = 1.0f/m_timestep;
			
		input.m_stepInfo = stepInfo;

		input.m_characterGravity.set(0,-9.8f,0);
		input.m_velocity = m_characterRigidBody->getRigidBody()->getLinearVelocity();
		input.m_position = m_characterRigidBody->getRigidBody()->getPosition();

		world->lock();
		m_characterRigidBody->checkSupport(stepInfo, input.m_surfaceInfo);

		// Only climb the ladder when the character is either unsupported or wants to go up.
		/*if ( atLadder && ( ( input.m_inputUD < 0 ) || ( input.m_surfaceInfo.m_supportedState != hkpSurfaceInfo::SUPPORTED ) ) )
		{
			hkVector4 right, ladderUp;
			right.setCross( UP, ladderNorm );
			ladderUp.setCross( ladderNorm, right );
			// Calculate the up vector for the ladder
			if (ladderUp.lengthSquared3() > HK_REAL_EPSILON)
			{
				ladderUp.normalize3();
			}

			// Reorient the forward vector so it points up along the ladder
			input.m_forward.addMul4( -ladderNorm.dot3(input.m_forward), ladderNorm);
			input.m_forward.add4( ladderUp );
			input.m_forward.normalize3();

			input.m_surfaceInfo.m_supportedState = hkpSurfaceInfo::UNSUPPORTED;
			input.m_surfaceInfo.m_surfaceNormal = ladderNorm;
			input.m_surfaceInfo.m_surfaceVelocity = ladderVelocity;
				
			HK_SET_OBJECT_COLOR( (hkUlong) m_characterRigidBody->getRigidBody()->getCollidable(), hkColor::rgbFromChars( 255, 255, 0, 100 ) );
		}
		else
		{
			// Change character rigid body color according to its state
			if( input.m_surfaceInfo.m_supportedState == hkpSurfaceInfo::SUPPORTED )
			{
				HK_SET_OBJECT_COLOR( (hkUlong) m_characterRigidBody->getRigidBody()->getCollidable(), hkColor::rgbFromChars( 0, 255, 0, 100 ) );
			}
			else
			{
				HK_SET_OBJECT_COLOR( (hkUlong) m_characterRigidBody->getRigidBody()->getCollidable(), hkColor::BLUE );
			}

		}*/
		HK_TIMER_END();
	}

	// Apply the character state machine
	{
		HK_TIMER_BEGIN( "update character state", HK_NULL );

		m_characterContext->update( input, output );

		HK_TIMER_END();
	}

	//Apply the player character controller
	{
		HK_TIMER_BEGIN( "simulate character", HK_NULL );

		// Set output velocity from state machine into character rigid body
		m_characterRigidBody->setLinearVelocity(output.m_velocity, m_timestep);

		HK_TIMER_END();

		world->unlock();
	}
}

hkVector4 PhysicsManager::GetPlayerPosition()
{
	return m_characterRigidBody->getPosition();
}

hkResult PhysicsManager::breakOffSubPart(const ContactImpulseLimitBreachedEvent& event, hkArray<hkpShapeKey>& keysBrokenOffOut, hkpPhysicsSystem& systemOut )
{
	return hkResult::HK_SUCCESS;
}

void PhysicsManager::RemoveBlock(PolyVox::Vector3DInt32 &chunk, PolyVox::Vector3DInt32 blockPos)
{
	hkpRigidBody* body = physicsMap[chunk];
	
	std::map<Vector3DInt32, hkpShapeKey> *keyMap = (std::map<Vector3DInt32, hkpShapeKey>*)body->getUserData();
	hkpShapeKey key = (*keyMap)[blockPos];

	world->markForWrite();
	m_breakUtil->removeKeysFromListShape(body, &key, 1);
	world->unmarkForWrite();
}

void PhysicsManager::SpawnCube(hkVector4 &pos)
{
	hkVector4 halfExtents(.25, .25, .25);
	hkpBoxShape *box = new hkpBoxShape(halfExtents);

	hkpRigidBodyCinfo info;
	info.m_shape = box;
	info.m_motionType = hkpMotion::MOTION_DYNAMIC;
	info.m_position = pos;

	hkpRigidBody* body = new hkpRigidBody(info);

	cubes.pushBack(body);

	world->lock();
	world->addEntity(body);
	world->unlock();
	body->removeReference();
	box->removeReference();
}
