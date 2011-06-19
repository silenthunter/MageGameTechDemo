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
#include <PolyVoxCore/MaterialDensityPair.h>
#include <PolyVoxCore/SimpleVolume.h>
#include <PolyVoxCore/CubicSurfaceExtractorWithNormals.h>
#include <PolyVoxCore/SurfaceMesh.h>
#include "GameGlobals.h"
#include <OIS.h>
#pragma endregion

#define HAVOK_VISUAL_DEBUGGER_ENABLED

using namespace PolyVox;

static void HK_CALL errorReport(const char* msg, void* userArgGivenToInit)
{
	printf("%s", msg);
}

///Calculates physics for the game world
/** Uses the Havok physics engine to compute the interactions between the player and the game world 
*/
class PhysicsManager
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

	int chunkSize;
	float worldScale;

	void initHavok();
	void deInitHavok();
	void stepVisualDebugger(float deltaTime);
	void registerVisualDebugger();
	void initPlayer();

	SimpleVolume<MaterialDensityPair44>* polyVolume;

public:
	PhysicsManager(SimpleVolume<MaterialDensityPair44>* volume);
	~PhysicsManager(void);

	///Updates a single chunk
	void UpdateChunk(Vector3DInt32& chunk);

	/**Calls UpdateChunk for a range of chunks
	    @param start The beginning corner of the region to be updated
		@param end The opposite corner of the region to be updated
		@note start coordinates must be <= the ones in end
		@sa UpdateChunk
	*/
	void UpdateChunkRange(Vector3DInt32& start, Vector3DInt32& end);
	void StepSimulation(float deltaTime);
	void UpdatePlayer(OIS::Keyboard* keyboard, OIS::Mouse* mouse, hkQuaternion &orientation);

	hkVector4 GetPlayerPosition();
};

#endif

