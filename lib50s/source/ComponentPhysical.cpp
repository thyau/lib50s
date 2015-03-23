/*
	ComponentPhysical.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Jon Schiavo
*/

#include "..\\include\\BulletCompat.hpp"
#include "..\\include\\ComponentPhysical.hpp"
#include "..\\include\\Entity.hpp"
#include "..\\include\\Scene.hpp"
#include "..\\include\\WindowsConsole.hpp"

using namespace std;

using namespace irr;
using namespace irr::core;

ComponentPhysical::ComponentPhysical() :
	BaseComponent(),
	m_World(NULL),
	m_Enabled(true),
	m_ShapeTransformsUpdated(true),
	m_ShapeCGChanged(true)
{
	// Create a 'blank' rigid body
	m_CompoundShape = new btCompoundShape();

	m_ShapeCGTransform.setIdentity();

	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, NULL, m_CompoundShape);
	//btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0f, NULL, new btBoxShape(btVector3(0.5f, 0.5f, 0.5f))); // DEBUG
	m_CompoundShape->calculateLocalInertia(rbInfo.m_mass, rbInfo.m_localInertia);
	rbInfo.m_startWorldTransform.setOrigin(BT_ZERO_VECTOR);

	m_Body = new btRigidBody(rbInfo);
}

ComponentPhysical::~ComponentPhysical()
{
	if(m_World)
	{
		m_World->removeRigidBody(m_Body);
		//m_Body->getBroadphaseHandle()->
	}

	SAFE_DELETE(m_Body);
	SAFE_DELETE(m_CompoundShape);
}

void ComponentPhysical::setLinearVelocity(const btVector3 &vel)
{
//	if(m_Body)
	m_Body->setLinearVelocity(vel);
}

void ComponentPhysical::setAngularVelocity(const btVector3 &vel)
{
//	if(m_Body)
	m_Body->setAngularVelocity(vel);
}

btVector3 ComponentPhysical::getLinearVelocity() const
{
//	if(m_Body)
		return btVector3(m_Body->getLinearVelocity());
//	else
//		return BT_ZERO_VECTOR;
}

btVector3 ComponentPhysical::getAngularVelocity() const
{
//	if(m_Body)
		return btVector3(m_Body->getAngularVelocity());
//	else
//		return BT_ZERO_VECTOR;
}

void ComponentPhysical::setEnabled(bool isEnabled)
{
	if (m_Enabled && m_World && !isEnabled)
	{
		removeBodyFromWorld();
	}
	else if (!m_Enabled && m_World && isEnabled)
	{
		addBodyToWorld();
	}

	m_Enabled = isEnabled;
}

void ComponentPhysical::update(float delta)
{
	if((bool)m_Parent /*&& (m_Body->getActivationState() == ACTIVE_TAG)*/) // Body can move a little bit even when it's not active
	{
		//boost::signals2::shared_connection_block blockRotation(m_Connections[ENT_SIGNAL_ROTATED]);
		//boost::signals2::shared_connection_block blockMove(m_Connections[ENT_SIGNAL_MOVED]);

		btTransform trans = m_Body->getCenterOfMassTransform() * m_ShapeCGTransform.inverse();

		m_Parent->setPositionQuiet(irrVec(trans.getOrigin()));
		m_Parent->setRotationQuiet(trans.getRotation());
	}
}

void ComponentPhysical::postAnimate(u32 timeMs)
{
	if (m_ShapeTransformsUpdated)
	{
		btScalar oldInvMass, oldMass, newMass;

		oldInvMass = m_Body->getInvMass();
		oldMass = (oldInvMass != 0)? btScalar(1.0)/oldInvMass : 0;

		newMass = 0;
		for (btScalar childMass : m_Masses)
		{
			newMass += childMass;
		}

		if (m_ShapeCGChanged)
		{
			if (newMass != 0)
			{
				btVector3 principalInertia;
				btTransform newChildTransform, oldCGTransform;

				oldCGTransform = m_ShapeCGTransform;

				// A fudge here: calculatePrincipalAxisTransform requires all masses to be zero, even though it's not strictly necessary
				std::vector<btScalar> fudgedMasses(m_Masses.size());
				for (unsigned int i = 0; i < m_Masses.size(); i++)
				{
					fudgedMasses[i] = (m_Masses[i] == 0)? 1.0e-12f : m_Masses[i];
				}

				m_CompoundShape->calculatePrincipalAxisTransform(fudgedMasses.data(), m_ShapeCGTransform, principalInertia);

				for (int i = 0; i < m_CompoundShape->getNumChildShapes(); i++)
				{
					newChildTransform = m_ShapeCGTransform.inverse() * oldCGTransform * m_CompoundShape->getChildTransform(i);
					m_CompoundShape->updateChildTransform(i, newChildTransform, false);
				}

				// Compensate for the shifted child transforms
				m_Body->setCenterOfMassTransform(m_Body->getCenterOfMassTransform() * oldCGTransform.inverse() * m_ShapeCGTransform);
			}

			PRINT("Shape CG changed, old mass = " << oldMass << ", new = " << newMass << endl, WHITE);

			m_ShapeCGChanged = false;
		}

		m_CompoundShape->recalculateLocalAabb();

		bool staticnessChanged = ((oldMass == 0) != (newMass == 0));

		// Body goes from static to dynamic or vice-versa, need to re-add it to the dynamics world
		if (staticnessChanged && m_World)
			removeBodyFromWorld();

		if ((abs(newMass - oldMass) > 1e-4) || (newMass != 0))
		{
			btVector3 localInertia;
			m_CompoundShape->calculateLocalInertia(newMass, localInertia);
			m_Body->setMassProps(newMass, localInertia);
			m_Body->updateInertiaTensor();
		}
		//PRINT("Mass props updated!" << endl, WHITE);

		if (staticnessChanged && m_World)
			addBodyToWorld();

		m_ShapeTransformsUpdated = false;

		// Flag body as active since children moved and can potentially create new collisions
		m_Body->setActivationState(ACTIVE_TAG);

		//PRINT("PostAnimate shape updated!" << endl, WHITE);
	}
}

void ComponentPhysical::sig_Moved(const vector3df& pos)
{
	btTransform t = m_Body->getCenterOfMassTransform();
	t.setOrigin(btVector3(pos.X, pos.Y, pos.Z) + m_ShapeCGTransform.getOrigin());
	m_Body->setCenterOfMassTransform(t);
}

void ComponentPhysical::sig_Rotated	(const vector3df& rot)
{
	btTransform t = m_Body->getCenterOfMassTransform() * m_ShapeCGTransform.inverse();
	btQuaternion q;
	q.setEuler(rot.Y * DEGTORAD, rot.X * DEGTORAD, rot.Z * DEGTORAD);
	t.setRotation(q);
	m_Body->setCenterOfMassTransform(t * m_ShapeCGTransform);
}

void ComponentPhysical::sig_SceneChange(const std::shared_ptr<Scene> &oldScene, const std::shared_ptr<Scene> newScene)
{
	moveScene(oldScene, newScene);
}

//void ComponentPhysical::sig_PreAnimate(u32 timeMs)
//{
//
//}

int	ComponentPhysical::addChildShape(const matrix4& absTrans, btCollisionShape *shape, btScalar mass)
{
	int id = -1;
	btTransform newTrans;

	matrix4 inverse;
	getInvAbsTransform(inverse);
	toBtTransform(inverse * absTrans, newTrans);

	m_CompoundShape->addChildShape(m_ShapeCGTransform.inverseTimes(newTrans), shape);
	m_Masses.push_back(mass);
	//PRINT("Shape updated from addChildShape" << endl, WHITE);
	m_ShapeTransformsUpdated = true;
	if (mass != 0)
	{
		m_ShapeCGChanged = true;
	}

	int idx = m_CompoundShape->getNumChildShapes() - 1;

	for (unsigned int i = 0; i < m_IndexList.size(); i++)
	{
		if (m_IndexList[i] < 0) // Find unused id
		{
			m_IndexList[i] = idx;
			id = i;
			break;
		}
	}

	if (id < 0)
	{
		m_IndexList.push_back(idx); // No unused ids, make a new one
		id = m_IndexList.size() - 1;
	}

	//PRINT("Adding child shape " << hex << shape << ", id = " << id << endl, WHITE);

	return id;
}

void ComponentPhysical::updateChildTransform(const matrix4& absTransform, int id, btScalar mass)
{
	btTransform oldTrans, newTrans;
	matrix4 inverse;

	oldTrans = m_CompoundShape->getChildTransform(m_IndexList[id]);

	getInvAbsTransform(inverse);
	toBtTransform(inverse * absTransform, newTrans);
	newTrans = m_ShapeCGTransform.inverseTimes(newTrans);

	if (!btTransformEqual(newTrans, oldTrans, 1.0e-5f))
	{
		/*PRINT("Shape updated from updateChildTransform, component " << hex << this << endl, WHITE);

		btMatrix3x3 R = oldTrans.getBasis();
		btVector3 t = oldTrans.getOrigin();
		PRINT("Old:" << endl << R[0][0] << " " << R[0][1] << " " << R[0][2] << endl
						     << R[1][0] << " " << R[1][1] << " " << R[1][2] << endl
							 << R[2][0] << " " << R[2][1] << " " << R[2][2] << endl
							 << t[0] << " " << t[1] << " " << t[2] << endl, WHITE);

		R = newTrans.getBasis();
		t = newTrans.getOrigin();
		PRINT("New:" << endl << R[0][0] << " " << R[0][1] << " " << R[0][2] << endl
						     << R[1][0] << " " << R[1][1] << " " << R[1][2] << endl
							 << R[2][0] << " " << R[2][1] << " " << R[2][2] << endl
							 << t[0] << " " << t[1] << " " << t[2] << endl, WHITE);

		PRINT("Abs (id=" << id << ":" << endl 
						     << absTransform(0,0) << " " << absTransform(0,1) << " " << absTransform(0,2) << " " << absTransform(0,3) << endl
							 << absTransform(1,0) << " " << absTransform(1,1) << " " << absTransform(1,2) << " " << absTransform(1,3) << endl
							 << absTransform(2,0) << " " << absTransform(2,1) << " " << absTransform(2,2) << " " << absTransform(2,3) << endl
							 << absTransform(3,0) << " " << absTransform(3,1) << " " << absTransform(3,2) << " " << absTransform(3,3) << endl, WHITE);

		matrix4 myAbsTransform = m_Parent->getAbsoluteTransformation();
		PRINT("My abs:" << endl 
						     << myAbsTransform(0,0) << " " << myAbsTransform(0,1) << " " << myAbsTransform(0,2) << " " << myAbsTransform(0,3) << endl
							 << myAbsTransform(1,0) << " " << myAbsTransform(1,1) << " " << myAbsTransform(1,2) << " " << myAbsTransform(1,3) << endl
							 << myAbsTransform(2,0) << " " << myAbsTransform(2,1) << " " << myAbsTransform(2,2) << " " << myAbsTransform(2,3) << endl
							 << myAbsTransform(3,0) << " " << myAbsTransform(3,1) << " " << myAbsTransform(3,2) << " " << myAbsTransform(3,3) << endl, WHITE);

		matrix4 myInvTransform = getInvAbsTransform();
		PRINT("My inv:" << endl 
						     << myInvTransform(0,0) << " " << myInvTransform(0,1) << " " << myInvTransform(0,2) << " " << myInvTransform(0,3) << endl
							 << myInvTransform(1,0) << " " << myInvTransform(1,1) << " " << myInvTransform(1,2) << " " << myInvTransform(1,3) << endl
							 << myInvTransform(2,0) << " " << myInvTransform(2,1) << " " << myInvTransform(2,2) << " " << myInvTransform(2,3) << endl
							 << myInvTransform(3,0) << " " << myInvTransform(3,1) << " " << myInvTransform(3,2) << " " << myInvTransform(3,3) << endl, WHITE);*/

		m_CompoundShape->updateChildTransform(m_IndexList[id], newTrans, false);
		m_ShapeTransformsUpdated = true;

		if (m_Masses[m_IndexList[id]] != 0)
		{
			m_ShapeCGChanged = true;
		}
	}

	if (m_Masses[m_IndexList[id]] != mass)
	{
		m_Masses[m_IndexList[id]] = mass;
		m_ShapeCGChanged = true;
	}
}

void ComponentPhysical::removeChildShape(int id)
{
	if (m_CompoundShape->getNumChildShapes() > 0)
	{
		m_CompoundShape->removeChildShapeByIndex(m_IndexList[id]);

		if (m_Masses[m_IndexList[id]] != 0)
		{
			m_ShapeCGChanged = true;
		}

		// NOTE!!! This code depends on the specific implementation of btCompoundShape::removeChildShapeByIndex
		// Sorry, I did not find another way to do this without resorting to clearing all shapes
		int lastIdx = m_CompoundShape->getNumChildShapes();
		m_Masses[m_IndexList[id]] = m_Masses[lastIdx];
		m_Masses.pop_back();
		for (unsigned int i = 0; i < m_IndexList.size(); i++)
		{
			if (m_IndexList[i] == lastIdx)
			{
				m_IndexList[i] = m_IndexList[id];
				break;
			}
		}
		m_IndexList[id] = -1;

		//PRINT("Shape with id " << id << " removed" << endl, WHITE);
	}
}

void ComponentPhysical::onAttach(const std::shared_ptr<Entity> &parent)
{
	BaseComponent::onAttach(parent);

	//m_Connections[ENT_SIGNAL_UPDATE] = m_Parent->subscribeUpdate(WO_CALL_FIRST, boost::bind(&ComponentPhysical::sig_Update, this, _1));
	m_Connections[ENT_SIGNAL_MOVED] = m_Parent->subscribeMoved(WO_CALL_FIRST, boost::bind(&ComponentPhysical::sig_Moved, this, _1));
	m_Connections[ENT_SIGNAL_ROTATED] = m_Parent->subscribeRotated(WO_CALL_FIRST, boost::bind(&ComponentPhysical::sig_Rotated, this, _1));
	m_Connections[ENT_SIGNAL_SCENECHANGE] = m_Parent->subscribeSceneChange(WO_CALL_FIRST, boost::bind(&ComponentPhysical::sig_SceneChange, this, _1, _2));
	//m_Connections[ENT_SIGNAL_PRE_ANIMATE] = m_Parent->subscribePreAnimate(WO_CALL_FIRST, boost::bind(&ComponentPhysical::sig_PreAnimate, this, _1));
	// Call second so we do our work after any ComponentCollider updates.
	//m_Connections[ENT_SIGNAL_POST_ANIMATE] = m_Parent->subscribePostAnimate(WO_CALL_SECOND, boost::bind(&ComponentPhysical::sig_PostAnimate, this, _1));

	// Make sure the body is in the right world
	//if((bool)m_Parent)
	//{
		Scene::Ptr scene(m_Parent->getScene().lock());
		if((bool)scene)
		{
			moveScene(Scene::Ptr(), scene);
		}
	//}

	//if(m_Body)
	//{
		m_Body->setUserPointer(m_Parent.get());
	//}

	parent->updateAbsolutePositionRecursive();
	sig_Moved(parent->getAbsolutePosition());
	sig_Rotated(parent->getAbsoluteTransformation().getRotationDegrees());

	Entity::fireSubtreeUpdates(parent.get());
}

void ComponentPhysical::onDetach(const std::shared_ptr<Entity> &parent)
{
	BaseComponent::onDetach(parent);

	//if(m_Body)
	//{
		m_Body->setUserPointer(NULL);
	//}

	Scene::Ptr scene(parent->getScene().lock());
	if((bool)scene)
	{
		moveScene(scene, Scene::Ptr());
	}

	Entity::fireSubtreeUpdates(parent.get());
}

void ComponentPhysical::moveScene(const std::shared_ptr<Scene> &oldScene, const std::shared_ptr<Scene> newScene)
{
	// If we have an old scene, remove the body from its world
	if((bool)oldScene && m_Enabled)
	{
		btDynamicsWorld* world = oldScene->getDynamicsWorld(); // TODO: eh, why aren't we using m_World?
		world->removeRigidBody(m_Body);
		PRINT("Removed rigid body from world." << endl, WHITE);
	}

	// If we have a new scene, add the body to the world
	if((bool)newScene)
	{
		m_World = newScene->getDynamicsWorld();

		if(m_Enabled)
		{
			addBodyToWorld();
			PRINT("Added rigid body to world." << endl, WHITE);
		}
	}
	else
	{
		m_World = NULL;
	}
}

const void ComponentPhysical::getInvAbsTransform(matrix4 &inverse)
{
	assert(m_Parent && "ComponentPhysical::getInvAbsTransform should not be called when not attached to an entity.");
	m_Parent->getAbsoluteTransformation().getInversePrimitive(inverse);
}

//void ComponentPhysical::setMass(btScalar mass)
//{
//	btVector3 localInertia;
//	m_CompoundShape->calculateLocalInertia(mass, localInertia);
//	m_Body->setMassProps(mass, localInertia);
//	m_Body->updateInertiaTensor();
//
//	PRINT("Set mass to " << mass << endl, WHITE);
//}

void ComponentPhysical::SpawnData::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	//TODO implement
}
	
void ComponentPhysical::SpawnData::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	//TODO implement
}