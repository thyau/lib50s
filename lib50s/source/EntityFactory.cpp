/*
	EntityFactory.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Jon Schiavo
*/

#include "..\\include\\EntityFactory.hpp"

long EntityFactory::m_NextSerialNum = 0;

EntityFactory::EntityFactory(irr::scene::ISceneManager* sm)
{
	m_SceneManager = sm;
}

EntityFactory::~EntityFactory()
{
}

ISceneNode* EntityFactory::addSceneNode(ESCENE_NODE_TYPE type, ISceneNode *parent)
{
	assert(type == ESNT_ENTITY && "EntityFactory::addSceneNode: unknown node type");

	// Let's not assign IDs at creation time, but leave it to the designers to assign
	// as a node attribute. That way we don't have to worry about an internal ID counter
	// being consistent with externally loaded world files.
	// We can check for ID uniqueness in the world editing stage.
	if (!parent) {
		parent = m_SceneManager->getRootSceneNode();
	}
	Entity *out = new Entity(parent, m_SceneManager, GetNextSerialNum());

	//PRINT("Entity ref count = " << out->getReferenceCount(), WHITE);
	out->drop();
	//PRINT("Entity ref count after drop = " << out->getReferenceCount(), WHITE);

	return out;
}

ISceneNode*	EntityFactory::addSceneNode(const c8 *typeName, ISceneNode *parent)
{
	// The scene loader will call us with arbitrary type names, and we return null to indicate that we don't recognize a type name
	//assert(strcmp(typeName, getCreateableSceneNodeTypeName(ESNT_ENTITY)) == 0 && "EntityFactory::addSceneNode: unknown node type name");
	return (strcmp(typeName, getCreateableSceneNodeTypeName(ESNT_ENTITY)) == 0)? addSceneNode(ESNT_ENTITY, parent) : NULL;
}

ESCENE_NODE_TYPE EntityFactory::getCreateableSceneNodeType(u32 idx) const
{
	assert(idx == 0 && "EntityFactory::getCreateableSceneNodeType: invalid type index.");
	return ESNT_ENTITY;
}

const c8* EntityFactory::getCreateableSceneNodeTypeName(u32 idx) const
{
	assert(idx == 0 && "EntityFactory::getCreateableSceneNodeTypeName: invalid type index.");
	return TYPE_NAME_ENTITY;
}

const c8* EntityFactory::getCreateableSceneNodeTypeName(ESCENE_NODE_TYPE type) const
{
	if (type == ESNT_ENTITY)
		return TYPE_NAME_ENTITY;
	else
		return NULL;
}

Entity::Ptr EntityFactory::makeEntity(ISceneNode *parent)
{
	ISceneNode *n = addSceneNode(ESNT_ENTITY, parent);
	return dynamic_cast<Entity*>(n)->getSharedPointer();
}

//Entity::Ptr EntityFactory::makeEntity(const ENTITY_SPAWN_DATA &spawn)
//{
//	Entity::Ptr out;
//	try
//	{
//		// Create the entity object
//		out = Entity::Ptr(new Entity(m_SceneManager->getRootSceneNode(), m_SceneManager, id));
//		out->setName(spawn.name.c_str());
//
//		// Is there a view component?
//		if(spawn.compView)
//		{
//			IComponent::Ptr p = createViewComponent(spawn.compView);
//			if(!(bool)p)
//				THROW("EntityFactory::makeEntity: Failed to create ComponentView");
//			if(!out->addComponent(p))
//				THROW("EntityFactory::makeEntity: Failed to add ComponentView to Entity");
//		}
//
//		// Is there a physical component?
//		if(spawn.compPhys)
//		{
//			IComponent::Ptr p = createPhysicalComponent(spawn.compPhys);
//			if(!(bool)p)
//				THROW("EntityFactory::makeEntity: Failed to create ComponentPhysical");
//			if(!out->addComponent(p))
//				THROW("EntityFactory::makeEntity: Failed to add ComponentPhysical to Entity");
//		}
//
//		//Are there script components?
//		for(std::vector<COMPONENT_SCRIPT_SPAWN_DATA>::const_iterator iter = spawn.compScript.begin(); iter != spawn.compScript.end(); ++iter)
//		{
//			IComponent::Ptr p = createScriptComponent(&(*iter));
//
//			if(!(bool)p)
//				THROW("EntityFactory::makeEntity: Failed to create ComponentScript");
//			if(!out->addComponent(p))
//				THROW("EntityFactory::makeEntity: Failed to add ComponentScript to Entity");
//		}
//
//		// Are there mesh components?
//		for(std::vector<COMPONENT_MESH_SPAWN_DATA>::const_iterator iter = spawn.compMesh.begin(); iter != spawn.compMesh.end(); ++iter)
//		{
//			IComponent::Ptr p = createMeshComponent(&(*iter));
//			if(!(bool)p)
//				THROW("EntityFactory::makeEntity: Failed to create ComponentMesh");
//			if(!out->addComponent(p))
//				THROW("EntityFactory::makeEntity: Failed to add ComponentMesh to Entity");
//		}
//
//		// Set initial position and rotation
//		out->setPosition(spawn.startPos);
//		out->setRotation(spawn.startRot);
//	}
//	catch(ERROR_DESC &e)
//	{
//		out.reset();
//	}
//
//	return out;
//}

