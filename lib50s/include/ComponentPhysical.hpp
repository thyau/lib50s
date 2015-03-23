#ifndef __COMPONENTPHYSICAL_HPP__
#define __COMPONENTPHYSICAL_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * The physics rigid body of an object
 */

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#include "IAttributes.h"

#include "..\\include\\BaseComponent.hpp"
#include "..\\include\\ComponentSpawnData.hpp"

#include "..\\include\\Entity.hpp"

/******************************************************
* Class  : ComponentPhysical
* Purpose: Links an Entity to the physics engine
* Notes  :
******************************************************/
class ComponentPhysical : public BaseComponent
{
	friend class ComponentFactory;
	friend class DebugScene; // Needs to access our rigid body and shape for drawing

public:
	typedef std::shared_ptr<ComponentPhysical> Ptr;
	typedef std::weak_ptr<ComponentPhysical> wPtr;

									ComponentPhysical	();
	virtual							~ComponentPhysical	();

	// Public Methods
	inline COMPONENT_TYPE			getType				() const override { return COMPONENT_PHYSICAL; }

	void							setLinearVelocity	(const btVector3 &vel);
	void							setAngularVelocity	(const btVector3 &vel);

	btVector3						getLinearVelocity	() const;
	btVector3						getAngularVelocity	() const;

	void							setEnabled			(bool isEnabled);

	virtual void					update				(float delta) override;
	virtual void					postAnimate			(irr::u32 timeMs);

	virtual void					sig_Moved			(const irr::core::vector3df& pos) override;
	virtual void					sig_Rotated			(const irr::core::vector3df& rot) override;
	virtual void					sig_SceneChange		(const std::shared_ptr<Scene> &oldScene, const std::shared_ptr<Scene> newScene);
	//virtual void					sig_PreAnimate		(u32 timeMs);

	// Methods for manipulating the compound shape

	// absTransform is the child node's absolute (world-coordinate) transform
	// Returns an id for future reference to this shape in updateChildTransform() and removeChildShape().
	virtual int						addChildShape		(const irr::core::matrix4& absTransform, btCollisionShape *shape, btScalar mass);
	virtual void					updateChildTransform(const irr::core::matrix4& absTransform, int id, btScalar mass);
	virtual void					removeChildShape	(int id);

	bool							isDynamic			(void) const { return (m_Body->getInvMass() != 0); }

	class SpawnData : public ComponentSpawnData
	{
	public:
		SpawnData() { }

		virtual COMPONENT_TYPE getSpawnType() const override { return COMPONENT_PHYSICAL; }

		virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options = 0) const;
		virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options = 0);

		// TODO: Collision group and mask
		// TODO: Angular Factor

		// If static, make sure that all colliders also have zero mass, or else the object can still move.
		bool m_IsStatic, m_IsKinematic, m_HasContactResponse;
		float /*m_Mass, */m_Friction, m_RollingFriction;
	};

protected:
	// Protected Methods
	virtual void					onAttach			(const std::shared_ptr<Entity> &parent) override;
	virtual void					onDetach			(const std::shared_ptr<Entity> &parent) override;

	void							moveScene			(const std::shared_ptr<Scene> &oldScene, const std::shared_ptr<Scene> newScene);

	// The return value of this function is guaranteed to be meaningful in sig_PostAnimate.
	// Otherwise you probably need to call updateAbsolutePositionRecursive() on the parent entity.
	const void						getInvAbsTransform	(irr::core::matrix4 &inverse);

	//// Protected so that user can't arbitrarily remove other Entities' child shapes
	//virtual void					clearChildShapes();

	//// Protected because mass should be specified in spawn data and not changed afterwards (ideally)
	//void							setMass				(btScalar mass);

	// Protected Members
	btRigidBody*					m_Body; // Never NULL
	btCompoundShape*				m_CompoundShape; // Never NULL
	std::vector<int>				m_IndexList; // See addChildShape()
	btDynamicsWorld*				m_World;
	bool							m_Enabled;

	// Whether child transforms of the compound shape were updated, requiring a recomputation of inertia tensor
	bool							m_ShapeTransformsUpdated;
	// Whether center of gravity has changed, requiring a recomputation of the CG transform and the inertia tensor
	bool							m_ShapeCGChanged;
	btTransform						m_ShapeCGTransform;
	std::vector<btScalar>			m_Masses;

private:
	inline void removeBodyFromWorld		(void)
	{
		m_World->removeRigidBody(m_Body);
	}

	inline void addBodyToWorld			(void)
	{
		m_World->addRigidBody(m_Body);
		m_World->updateSingleAabb(m_Body); // Seems to improve performance
		m_World->updateAabbs();
	}
};

#endif