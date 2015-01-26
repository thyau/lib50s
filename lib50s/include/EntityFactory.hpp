#ifndef __ENTITYFACTORY_HPP__
#define __ENTITYFACTORY_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Factory-pattern for creating Entity objects
 */

#include "ISceneNodeFactory.h"

#include "..\\include\\Entity.hpp"

/*
 * STRUCTURE: ENTITY_SPAWN_DATA
 * Data record for creating a new object
 */
//typedef struct ENTITY_SPAWN_DATA
//{
//	std::string name;
//
//	vector3df startPos;
//	vector3df startRot;
//
//	// Component spawn data
//	// Note: We use a pointer for compPhys because it's a convenient way to indicate its
//	//       presence.  This should usually be a pointer to a stack variable, since in
//	//       general this will only be needed for a short time.
//	//       For components that we can have multiple of, we use a vector.  Note that the
//	//       instances in the vector are NOT pointers.  This is because we can use emplace_back
//	//       to avoid copying overhead, and we don't need to maintain an array of objects in the
//	//       callers scope, or remember to deallocate the structs on the heap when we are done.
//	COMPONENT_VIEW_SPAWN_DATA* compView;
//	COMPONENT_PHYSICAL_SPAWN_DATA* compPhys;
//	std::vector<COMPONENT_MESH_SPAWN_DATA> compMesh;
//	std::vector<COMPONENT_SCRIPT_SPAWN_DATA> compScript;
//
//	ENTITY_SPAWN_DATA() :
//		name(""),
//		startPos(ZERO_VECTOR),
//		startRot(ZERO_VECTOR),
//		compView(NULL),
//		compPhys(NULL)
//	{ }
//
//	// Note: We will not pass vectors of spawn data through this constructor, there is too
//	//       much unecessary performance penalty.  Create the ENTITY_SPAWN_DATA object, then
//	//       fill the vector directly.
//	ENTITY_SPAWN_DATA(
//		const std::string &_name, 
//		const vector3df &pos, 
//		const vector3df &rot, 
//		COMPONENT_VIEW_SPAWN_DATA* view, 
//		COMPONENT_PHYSICAL_SPAWN_DATA* phys) :
//			name(_name),
//			startPos(pos),
//			startRot(rot),
//			compView(view),
//			compPhys(phys)
//	{ }
//
//} ENTITY_SPAWN_DATA;

#define TYPE_NAME_ENTITY "entity"

/******************************************************
* Class  : EntityFactory
* Purpose: Automates creation of Entity objects
* Notes  :
******************************************************/
class EntityFactory : public ISceneNodeFactory
{
public:
								EntityFactory					(irr::scene::ISceneManager* sm);
	virtual						~EntityFactory					();

	// Public Methods

	// adds a scene node to the scene graph based on its type id 
	virtual ISceneNode*			addSceneNode					(ESCENE_NODE_TYPE type, ISceneNode *parent=0) override;
	// adds a scene node to the scene graph based on its type name
	virtual ISceneNode*			addSceneNode					(const c8 *typeName, ISceneNode *parent=0) override;
	// returns amount of scene node types this factory is able to create
	virtual u32					getCreatableSceneNodeTypeCount	() const override { return 1; }
	// returns type of a createable scene node type
	virtual ESCENE_NODE_TYPE	getCreateableSceneNodeType		(u32 idx) const override;
	// returns type name of a createable scene node type by index
	virtual const c8*			getCreateableSceneNodeTypeName	(u32 idx) const override;
	// returns type name of a createable scene node type 
	virtual const c8*			getCreateableSceneNodeTypeName	(ESCENE_NODE_TYPE type) const override;
	
	// Convenience function
	Entity::Ptr					makeEntity						(ISceneNode *parent=0);
	
	inline long					GetNextSerialNum				(void)									{ return m_NextSerialNum++; }	

	inline irr::scene::ISceneManager* getSceneManager() { return this->m_SceneManager; }

private:

	static long m_NextSerialNum;

	// Private members
	irr::scene::ISceneManager*	m_SceneManager;  // Not owning
};

#endif