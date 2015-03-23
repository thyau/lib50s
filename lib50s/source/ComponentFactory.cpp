/*
	ComponentFactory.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Timothy Yau
*/

#include "..\include\ComponentFactory.hpp"

using namespace std;

using namespace irr::scene;

ComponentFactory::ComponentFactory(ISceneManager* mgr)
	: m_SceneManager(mgr)
{
}


ComponentFactory::~ComponentFactory(void)
{
}

IComponent::Ptr ComponentFactory::createComponent(const ComponentSpawnData *data) {
	// I think it's better to not create a component at all than to return a null pointer and risk segfaulting
	assert(data && "ComponentFactory::createComponent: NULL spawn data");

	IComponent::Ptr out;

	switch (data->getSpawnType())
	{
	case COMPONENT_VIEW:
		out = createViewComponent(dynamic_cast<const ComponentView::SpawnData*>(data));
		break;

	case COMPONENT_PHYSICAL:
		out = createPhysicalComponent(dynamic_cast<const ComponentPhysical::SpawnData*>(data));
		break;

	case COMPONENT_COLLIDER:
		out = createColliderComponent(dynamic_cast<const ComponentCollider::SpawnData*>(data));
		break;

	case COMPONENT_MESH:
		out = createMeshComponent(dynamic_cast<const ComponentMesh::SpawnData*>(data));
		break;

	case COMPONENT_SCRIPT:
		out = createScriptComponent(dynamic_cast<const ComponentScript::SpawnData*>(data));
		break;

	default:
		assert(false && "ComponentFactory::createComponent: unknown component type");
		break;
	}

	return out;
}

void ComponentFactory::cleanPhysicsShapeCache()
{
	ShapeMap::iterator it = m_ShapeCache.begin();
	while (it != m_ShapeCache.end())
	{
		if (it->second.expired())
		{
			m_ShapeCache.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

IComponent::Ptr ComponentFactory::createViewComponent(const ComponentView::SpawnData* data)
{
	ComponentView* v = new ComponentView();

	//ICameraSceneNode* cam = m_SceneManager->addCameraSceneNode(NULL, data->posOffset, data->lookAt, data->active);
	//cam->bindTargetAndRotation(true);
	//v->setNode(cam);

	return IComponent::Ptr(v);
}

IComponent::Ptr ComponentFactory::createPhysicalComponent(const ComponentPhysical::SpawnData* data)
{
	ComponentPhysical* p = new ComponentPhysical();

	//// Initially start on the origin
	//// After this component is attached the object will be moved to its starting location
	//btTransform trans;
	//trans.setIdentity();
	//trans.setOrigin(BT_ZERO_VECTOR);

	//btDefaultMotionState *ms = new btDefaultMotionState(trans);

	//// Create collision shape and inertia
	//btCollisionShape *shape = new btBoxShape(btVector3(0.5,0.5,0.5));
	//btVector3 inertia;
	//shape->calculateLocalInertia(data->mass, inertia);

	//// Create the rigid body
	//// Note: seems to be a bug with Bullet.  You must use btRigidBodyConstructionInfo.
	////		the constructor with the same arguments doesn't seem to work
	//btRigidBody::btRigidBodyConstructionInfo rbInfo(data->mass, ms, shape, inertia);
	//btRigidBody* body = new btRigidBody(rbInfo);

	//// Set initial velocities
	//body->setLinearVelocity(data->startLinearVel);
	//body->setAngularVelocity(data->startAngularVel);

	//// Give the body to the component
	//p->setBody(body);

	p->m_Body->setFriction(data->m_Friction);
	p->m_Body->setRollingFriction(data->m_RollingFriction);

	if (data->m_IsStatic)
		p->m_Body->setCollisionFlags(p->m_Body->getCollisionFlags() | (btCollisionObject::CF_STATIC_OBJECT));
	if (data->m_IsKinematic)
		p->m_Body->setCollisionFlags(p->m_Body->getCollisionFlags() | (btCollisionObject::CF_KINEMATIC_OBJECT));
	if (!data->m_HasContactResponse)
		p->m_Body->setCollisionFlags(p->m_Body->getCollisionFlags() | (btCollisionObject::CF_NO_CONTACT_RESPONSE));

	//p->m_Body->getCollisionShape()->setMargin(

	return IComponent::Ptr(p);
}

IComponent::Ptr ComponentFactory::createColliderComponent(const ComponentCollider::SpawnData* data)
{
	ComponentCollider* p = new ComponentCollider();

	if (m_CopySpawnData) 
	{
		p->m_SpawnData = new ComponentCollider::SpawnData();
		*(p->m_SpawnData) = *data;
	}

	if (!data->m_DataID.empty())
	{
		ShapeMap::iterator it = m_ShapeCache.find(data->m_DataID);

		if (it != m_ShapeCache.end())
		{
			// Might fail if expired
			p->m_Shape = it->second.lock();
		}
	}

	if (!p->m_Shape)
	{
		// Cache lookup failed
		btCollisionShape *shape = data->createCollisionShape();

		shared_ptr<btCollisionShape> shapePtr(shape);
		if (!data->m_DataID.empty())
			m_ShapeCache.insert(ShapeMap::value_type(data->m_DataID, weak_ptr<btCollisionShape>(shapePtr)));

		p->m_Shape = shapePtr;
	}

	p->m_IsStatic = data->m_IsStatic;
	p->m_Mass = data->m_Mass;

	return IComponent::Ptr(p);
}

IComponent::Ptr ComponentFactory::createMeshComponent(const ComponentMesh::SpawnData* data)
{
	ComponentMesh* m = new ComponentMesh();

	/*IMeshSceneNode* node = NULL;
	try
	{
		switch(data->meshType)
		{
		case COMPONENT_MESH_SPAWN_DATA::TYPE_PRIMITIVE_CUBE:
			node = m_SceneManager->addCubeSceneNode(
				1.0f,
				NULL,
				-1,
				data->posOffset,
				data->rotOffset,
				data->scale
				);
			break;

		case COMPONENT_MESH_SPAWN_DATA::TYPE_PRIMITIVE_SPHERE:
			node = m_SceneManager->addSphereSceneNode(
				0.5,
				16,
				NULL,
				-1,
				data->posOffset,
				data->rotOffset,
				data->scale
				);
			break;

		case COMPONENT_MESH_SPAWN_DATA::TYPE_MESH:
			node = NULL;
			break;

		default:
			THROW("EntityFactory::createMeshComponent: Invalid mesh type");
		}
	}
	catch(ERROR_DESC &e)
	{
		SAFE_DELETE(m);
		return IComponent::Ptr();
	}

	m->setNode(node);*/

	return IComponent::Ptr(m);
}

IComponent::Ptr ComponentFactory::createScriptComponent(const ComponentScript::SpawnData* data)
{
	ComponentScript* script = new ComponentScript();
	
	//script->setLuaState(data->state);
	//script->initializeLuaSide();

	return IComponent::Ptr(script);
}
