#ifndef __COMPONENTMESH_HPP__
#define __COMPONENTMESH_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Components that represent renderable portions of the object
 */

#include "..\\include\\ComponentWithSceneNode.hpp"
#include "..\\include\\ComponentSpawnData.hpp"

//typedef struct COMPONENT_MESH_SPAWN_DATA
//{
//	enum TYPE
//	{
//		TYPE_PRIMITIVE_CUBE,
//		TYPE_PRIMITIVE_SPHERE,
//		TYPE_MESH
//	};
//
//	TYPE meshType;
//	std::string meshName;
//	vector3df posOffset;
//	vector3df rotOffset;
//	vector3df scale;
//
//	COMPONENT_MESH_SPAWN_DATA() :
//		meshType(TYPE_PRIMITIVE_CUBE),
//		meshName(""),
//		posOffset(ZERO_VECTOR),
//		rotOffset(ZERO_VECTOR),
//		scale(1,1,1)
//	{ }
//
//	COMPONENT_MESH_SPAWN_DATA(
//		TYPE t, 
//		const std::string &_meshName, 
//		const vector3df &pos,
//		const vector3df &rot,
//		const vector3df &_scale
//		) :
//			meshType(t),
//			meshName(meshName),
//			posOffset(pos),
//			rotOffset(rot),
//			scale(_scale)
//	{ }
//} COMPONENT_MESH_SPAWN_DATA;

/******************************************************
* Class  : ComponentMesh
* Purpose: Wrapper for a renderable scene node
* Notes  :
******************************************************/
class ComponentMesh : public ComponentWithSceneNode
{
public:
	typedef std::shared_ptr<ComponentMesh> Ptr;
	typedef std::weak_ptr<ComponentMesh> wPtr;

							ComponentMesh			();
	virtual					~ComponentMesh			();

	// Public Methods
	virtual inline bool		allowMultiple			() const override { return true; }
	inline COMPONENT_TYPE	getType					() const override { return COMPONENT_MESH; }

	// Need to allow only specific types of renderable scene nodes, so we overload
	// each individually
	ISceneNode*				setNode					(IAnimatedMeshSceneNode* node) { return this->setNodeImpl(node); }
	ISceneNode*				setNode					(IMeshSceneNode* node) { return this->setNodeImpl(node); }

	class SpawnData : public ComponentSpawnData
	{
	public:
		SpawnData() { }

		virtual COMPONENT_TYPE getSpawnType() const override { return COMPONENT_MESH; }

		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

		// TODO attributes to reference a mesh node that's a child of the parent entity
	};

protected:
	// Protected Methods
	virtual void			onAttach				(const std::shared_ptr<Entity> &parent) override;
	virtual void			onDetach				(const std::shared_ptr<Entity> &parent) override;
};

#endif