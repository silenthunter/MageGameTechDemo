#pragma once

#ifndef PHYSICSMANAGER
#define PHYSICSMANAGER

#pragma region Includes
// Math and base include
#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Monitor/hkMonitorStream.h>
#include <Common/Base/Memory/System/hkMemorySystem.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>

/////////////////////////////////////////////////////////////////////////////////////////////// Dynamics includes
#include <Physics/Collide/hkpCollide.h>	
#include <Physics/Collide/Agent/ConvexAgent/SphereBox/hkpSphereBoxAgent.h>	
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>					
#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>				
#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>					

#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastInput.h>			
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>			

#include <Physics/Dynamics/World/hkpWorld.h>								
#include <Physics/Dynamics/Entity/hkpRigidBody.h>							
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>	

#include <Common/Base/Thread/Job/ThreadPool/Cpu/hkCpuJobThreadPool.h>
#include <Common/Base/Thread/Job/ThreadPool/Spu/hkSpuJobThreadPool.h>
#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

/////////////////////////////////////////////////////////////////////////////////////////////// Visual Debugger includes
#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>				

/////////////////////////////////////////////////////////////////////////////////////////////// Keycode

/////////////////////////////////////////////////////////////////////////////////////////////// BvTree
#include <Physics/Collide/Shape/Compound/Collection/ExtendedMeshShape/hkpExtendedMeshShape.h>
#include <Physics/Internal/Collide/Mopp/Code/hkpMoppCode.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppUtility.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppCompilerInput.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppBvTreeShape.h>

#include <Physics/Collide/Shape/Compound/Tree/hkpBvTreeShape.h>

#include <Physics/Collide/Shape/Convex/ConvexTransform/hkpConvexTransformShape.h>
#include <Physics/Collide/Shape/Compound/Collection/List/hkpListShape.h>
#include <Physics/Utilities/Destruction/BreakOffParts/hkpBreakOffPartsUtil.h>

/////////////////////////////////////////////////////////////////////////////////////////////// Character Control
#include <Physics/Utilities/CharacterControl/hkpCharacterControl.h>
#include <Physics/Utilities/CharacterControl/hkpCharacterControllerCinfo.h>
#include <Physics/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBody.h>
#include <Physics/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBodyCinfo.h>
#include <Physics/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBodyListener.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>

#include <Physics/Utilities/CharacterControl/StateMachine/hkpCharacterState.h>
#include <Physics/Utilities/CharacterControl/StateMachine/hkpCharacterContext.h>
#include <Physics/Utilities/CharacterControl/StateMachine/hkpCharacterStateManager.h>
#include <Physics/Utilities/CharacterControl/StateMachine/Climbing/hkpCharacterStateClimbing.h>
#include <Physics/Utilities/CharacterControl/StateMachine/Flying/hkpCharacterStateFlying.h>
#include <Physics/Utilities/CharacterControl/StateMachine/InAir/hkpCharacterStateInAir.h>
#include <Physics/Utilities/CharacterControl/StateMachine/Jumping/hkpCharacterStateJumping.h>
#include <Physics/Utilities/CharacterControl/StateMachine/OnGround/hkpCharacterStateOnGround.h>

#include <stdio.h>
#include <vector>
#include <PolyVoxCore/SimpleVolume.h>
#include <PolyVoxCore/CubicSurfaceExtractor.h>
#include <PolyVoxCore/SurfaceMesh.h>
#include "GameGlobals.h"
#include "GraphicsManager.h"
#include <OIS.h>
#include "GameTimer.h"
#pragma endregion

#define HAVOK_VISUAL_DEBUGGER_ENABLED

static void HK_CALL errorReport(const char* msg, void* userArgGivenToInit)
{
	printf("%s", msg);
}

typedef void (*ProgressCallback)(void);

///Calculates physics for the game world
/** Uses the Havok physics engine to compute the interactions between the player and the game world 
*/
class PhysicsManager : public hkpBreakOffPartsListener
{
private:
	//Variables for memory needs of HAVOK
	hkMemoryRouter* m_pMemoryRouter;
	hkHardwareInfo m_hardwareInfo;
	int m_iTotalNumThreadsUsed;
	hkCpuJobThreadPoolCinfo m_threadPoolCinfo;
	hkJobThreadPool* m_pThreadPool;
	hkJobQueueCinfo m_jobQueuInfo;
	hkJobQueue* m_pJobQueue;

	//Visual debugger
	bool m_bVisualDebuggerActive;
	hkArray<hkProcessContext*> m_arrayPhysicsContext;
	hkVisualDebugger* m_pVisualDebugger;
	hkpPhysicsContext* m_pPhysicsContext;

	hkpWorldCinfo m_worldInfo;

	hkpWorld *world;///< The world that contains the landscape and player body
	hkpCharacterRigidBody* m_characterRigidBody;
	hkpCharacterContext* m_characterContext;

	hkpBreakOffPartsUtil* m_breakUtil;

	std::map<PolyVox::Vector3DInt32, hkpRigidBody*> physicsMap;
	hkArray<hkpRigidBody*> cubes;
	hkArray<PolyVox::Vector3DInt32> pageQueue;

	int chunkSize;
	int maxPageSize;
	int maxRender;
	float worldScale;

	void initHavok();
	void deInitHavok();
	void stepVisualDebugger(float deltaTime);
	void registerVisualDebugger();
	void initPlayer();

	PolyVox::SimpleVolume<VoxelMat>* polyVolume;

	GraphicsManager* graphMan;

public:
	PhysicsManager(PolyVox::SimpleVolume<VoxelMat>* volume, GraphicsManager *manager = NULL);
	~PhysicsManager(void);

	///Updates a single chunk
	void UpdateChunk(PolyVox::Vector3DInt32& chunk);

	/**Calls UpdateChunk for a range of chunks
	@param start The beginning corner of the region to be updated
	@param end The opposite corner of the region to be updated
	@note start coordinates must be <= the ones in end
	@sa UpdateChunk
	*/
	void UpdateChunkRange(PolyVox::Vector3DInt32& start, PolyVox::Vector3DInt32& end);

	/**
	@brief Updates the Havok by a specified time
	@details A multithreaded step is called, advancing the physics engine by the inputed time
	@param deltaTime How much time to simulate
	@note Havok runs optimally using a timestep of 1.f/60.f
	*/
	void StepSimulation(float deltaTime);

	/**
	@brief Updates Havok's physics character based on input passed in from OIS objects
	@param keyboard Keyboard state
	@param mouse Mouse state
	@param orientation Current orientation of the Ogre character in the game world
	*/
	void UpdatePlayer(OIS::Keyboard* keyboard, OIS::Mouse* mouse, hkQuaternion &orientation, float elapsed);

	/**
	@brief Returns the physics character's position
	@return Player position
	*/
	hkVector4 GetPlayerPosition();

	/**
	@brief Removes a the physics for a block at a given location
	@param chunk The chunk position
	@param blockPos The absolute position for the block within the voxel volume
	*/
	void RemoveBlock(PolyVox::Vector3DInt32 &chunk, PolyVox::Vector3DInt32 blockPos);

	/**
	@brief Spawns a dynamic motion physics cube
	@param position The position the cube will be spawned
	*/
	void SpawnCube(hkVector4 &position);

	virtual hkResult breakOffSubPart(   const ContactImpulseLimitBreachedEvent& event, hkArray<hkpShapeKey>& keysBrokenOffOut, hkpPhysicsSystem& systemOut );
};

#endif

