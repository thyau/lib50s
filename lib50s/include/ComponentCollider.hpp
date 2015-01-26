#ifndef __COMPONENT_COLLIDER_HPP__
#define __COMPONENT_COLLIDER_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * The collision shape to be assigned to a physics rigid body
 */


#include "BaseComponent.hpp"
#include "ComponentSpawnData.hpp"

#include "ComponentPhysical.hpp"
#include "Entity.hpp"

class ComponentPhysical;

class ComponentCollider : public BaseComponent
{
	friend class ComponentFactory;

public:
	typedef std::shared_ptr<ComponentCollider> Ptr;
	typedef std::weak_ptr<ComponentCollider> wPtr;

	ComponentCollider(void);
	virtual ~ComponentCollider(void);

	virtual inline COMPONENT_TYPE	getType				() const { return COMPONENT_COLLIDER; }

	virtual void					onAttach			(const std::shared_ptr<Entity> &parent) override;
	virtual void					onDetach			(const std::shared_ptr<Entity> &parent) override;

	virtual void					postAnimate			(u32 timeMs);

	virtual void					sig_SubtreeChange	(ISceneNode *root);

	class SpawnData : public ComponentSpawnData
	{
	public:
		SpawnData() { }

		virtual COMPONENT_TYPE getSpawnType() const override { return COMPONENT_COLLIDER; }

		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

		virtual btCollisionShape* createCollisionShape() const;

		BroadphaseNativeTypes m_ShapeType;
		// For STATIC_PLANE_PROXYTYPE
		btVector3 m_PlaneNormal;
		float m_PlaneConstant;
		// For SPHERE_SHAPE_PROXYTYPE, CYLINDER_SHAPE_PROXYTYPE, CAPSULE_SHAPE_PROXYTYPE, BOX_SHAPE_PROXYTYPE
		btVector3 m_BoundingBoxSize; // axis-aligned box, x/y/z dimensions
		// For CYLINDER_SHAPE_PROXYTYPE, CAPSULE_SHAPE_PROXYTYPE.
		// 0, 1, 2 means align to the X, Y, Z axis respectively.
		int m_Orientation;

		std::string m_ExternalURI; // For e.g. loading a mesh file

		// Used to identify the original source of the data when one scene is imported into another.
		// This allows us to share shape memory when multiple copies of a file is imported into 
		// the main scene.
		// Should be set to the empty string when the data is modified after import.
		std::string m_DataID;

		// Specifies that this shape will never move with respect to the ancestor representative 
		// compound shape, and therefore the child shape transform will not be updated.
		// For performance when creating complex compound shapes.
		// NOTE: If set to true, the parent Entity must be moved and/or rotated AFTER this component 
		// has been added, or else the shape's transform will not be updated.
		bool m_IsStatic;

		// Mass of this collider, to take effect when part of an ancestor Entity's ComponentPhysical's
		// compound shape rigid body. Note that updating the positions of non-static, non-zero-mass
		// colliders within a compound shape is expensive. Therefore if a collider must frequently
		// move relative to its parent, set the mass to zero if possible.
		btScalar m_Mass;
	};

protected:

	void	detachShape				(void);
	// Try to obtain the ComponentPhysical from the given Entity, and attach our shape to the
	// CompoundShape there. Returns false if the Entity has no ComponentPhysical.
	// e must be an ancestor of our parent entity.
	bool	attachShape				(shared_ptr<Entity> e);
	void	reattachShape			(void);

	//virtual void processTreeUpdate(ISceneNode* root);

	shared_ptr<btCollisionShape> m_Shape;

	// ID from attaching to a ComponentPhysical's CompoundShape.
	// -1 means not attached
	int m_ChildShapeID;
	// The ComponentPhysical that represents the body our shape belongs to
	shared_ptr<ComponentPhysical> m_CompPhysicalBody;

	bool m_IsStatic;
	btScalar m_Mass;
};

#endif