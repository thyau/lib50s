#ifndef __SCENE_HPP__
#define __SCENE_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Convenient way of loading and grouping entities into a container
 * Each scene has its own physics world.  Objects in one scene cannot interact
 * with objects in another
 */

#include "IMesh.h"

#include "..\\include\\Entity.hpp"

/******************************************************
* Class  : Scene
* Purpose: A collection of entities and their physical world
* Notes  :
******************************************************/
class Scene : public std::enable_shared_from_this<Scene>
{
public:
	typedef std::shared_ptr<Scene> Ptr;
	typedef std::weak_ptr<Scene> wPtr;

	typedef std::map<long, Entity::Ptr> EntityMap;

								Scene					();
								~Scene					();

	// Public Methods
	inline btCollisionConfiguration*	getCollisionConfiguration	() const { return m_CollisionConfig; }
	inline btBroadphaseInterface*		getBroadPhase				() const { return m_Broadphase; }
	inline btCollisionDispatcher*		getCollisionDispatcher		() const { return m_CollisionDispatcher; }
	inline btConstraintSolver*			getConstraintSolver			() const { return m_ConstraintSolver; }
	inline btDynamicsWorld*				getDynamicsWorld			() const { return m_World; }

	bool						addEntity				(const Entity::Ptr &e);
	bool						addEntity				(irr::scene::ISceneNode *e);
	Entity::Ptr					getEntity				(long serialNum);
	bool						removeEntity			(long serialNum);
	void						removeAllEntities		(void);

	void						updateAll				(float delta);

	static btTriangleMesh* convertToBtMesh(irr::scene::IMesh* pMesh, const irr::core::vector3df& scaling);

protected:
	void						processPhysicsUpdate	(float timestep);
	static void					physicsTickCallback		(btDynamicsWorld* world, btScalar timestep);

	// Protected Members
	btCollisionConfiguration*	m_CollisionConfig;
	btBroadphaseInterface*		m_Broadphase;
	btCollisionDispatcher*		m_CollisionDispatcher;
	btConstraintSolver*			m_ConstraintSolver;
	btDynamicsWorld*			m_World;

	EntityMap					m_Entities;
};

#endif