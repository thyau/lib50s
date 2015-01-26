/*
	Scene.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Jon Schiavo
*/

#include "..\\include\\Scene.hpp"

Scene::Scene()
{
	m_CollisionConfig = new btDefaultCollisionConfiguration();
	m_Broadphase = new btAxisSweep3(btVector3(-1000, -1000, -1000), btVector3(1000, 1000, 1000));
	m_CollisionDispatcher = new btCollisionDispatcher(m_CollisionConfig);
	m_ConstraintSolver = new btSequentialImpulseConstraintSolver();
	m_World = new btDiscreteDynamicsWorld(m_CollisionDispatcher, m_Broadphase, m_ConstraintSolver, m_CollisionConfig);

	m_World->setGravity(btVector3(0,-9.8f,0));
	//m_World->setInternalTickCallback(Scene::physicsTickCallback, static_cast<void*>(this));
}

Scene::~Scene()
{
	//PRINT("Scene destructing... " << endl, WHITE);

	//for(EntityMap::iterator iter = m_Entities.begin(); iter != m_Entities.end(); ++iter)
	//{
	//	PRINT("Entity " << hex << iter->second.get() << " irr refs: " << iter->second->getReferenceCount() << ", shared ptr: " << iter->second.use_count() << endl, WHITE);
	//}
	

	SAFE_DELETE(m_World);
	SAFE_DELETE(m_ConstraintSolver);
	SAFE_DELETE(m_CollisionDispatcher);
	SAFE_DELETE(m_Broadphase);
	SAFE_DELETE(m_CollisionConfig);
}

bool Scene::addEntity(const Entity::Ptr &e)
{
	if(!(bool)e)
		return false;

	e->setScene(shared_from_this());

	// Check that this entity is not already in the scene
	if(m_Entities.find(e->getSerialNum()) != m_Entities.end())
		return false;

	m_Entities.insert(pair<long, Entity::Ptr>(e->getSerialNum(), e));

	return true;
}

bool Scene::addEntity(ISceneNode *e)
{
	Entity *ent = dynamic_cast<Entity*>(e);
	assert(ent && "Scene::addEntity: scene node could not be downcasted to entity.");

	return addEntity(ent->getSharedPointer());
}

Entity::Ptr Scene::getEntity(long serialNum)
{
	EntityMap::iterator found = m_Entities.find(serialNum);
	if(found == m_Entities.end())
	{
		PRINT("ERROR: Could not find object "<<serialNum<<endl, RED);
		return Entity::Ptr();
	}
	else
		return found->second;
}

bool Scene::removeEntity(long serialNum)
{
	EntityMap::iterator found = m_Entities.find(serialNum);
	if(found != m_Entities.end())
	{
		(*found).second->setScene(Ptr());
		m_Entities.erase(found);
		return true;
	}
	else
		return false;
}

void Scene::removeAllEntities(void)
{
	for (EntityMap::iterator it = m_Entities.begin(); it != m_Entities.end(); it++)
	{
		(*it).second->setScene(Ptr());
	}
	m_Entities.clear();
}

void Scene::updateAll(float delta)
{
	// Step the physics simulation
	m_World->stepSimulation(delta, 3);

	// Update all of the objects with their new physical positions
	for(EntityMap::iterator iter = m_Entities.begin(); iter != m_Entities.end(); ++iter)
	{
		iter->second->update(delta);
	}
}

void Scene::processPhysicsUpdate(float timestep)
{
	for(EntityMap::iterator iter = m_Entities.begin(); iter != m_Entities.end(); ++iter)
	{
		iter->second->physicsUpdate(timestep);
	}
}

void Scene::physicsTickCallback(btDynamicsWorld* world, btScalar timestep)
{
	Scene* s = static_cast<Scene*>(world->getWorldUserInfo());
	if(!s)
		return;

	s->processPhysicsUpdate(timestep);
}

// Adapted from http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?t=8141
btTriangleMesh* Scene::convertToBtMesh(IMesh* pMesh, const core::vector3df& scaling)
{
	btVector3 vertices[3];
	u32 i,j,k,index,numVertices,numIndices;
	u16* mb_indices;

	btTriangleMesh *pTriMesh = new btTriangleMesh();

	for (i=0; i<pMesh->getMeshBufferCount(); i++)
	{
		scene::IMeshBuffer* mb=pMesh->getMeshBuffer(i);

		if(mb->getVertexType()==video::EVT_STANDARD)
		{
			video::S3DVertex* mb_vertices=(video::S3DVertex*)mb->getVertices();
			mb_indices = mb->getIndices();
			numVertices = mb->getVertexCount();
			numIndices = mb->getIndexCount();

			for(j=0;j<numIndices;j+=3)
			{
				for (k=0;k<3;k++)
				{
					index = mb_indices[j+k];
					vertices[k] = btVector3(mb_vertices[index].Pos.X*scaling.X, mb_vertices[index].Pos.Y*scaling.Y, mb_vertices[index].Pos.Z*scaling.Z);
				}
				pTriMesh->addTriangle(vertices[0], vertices[1], vertices[2]);
			}
		}
		else if(mb->getVertexType()==video::EVT_2TCOORDS)
		{
			video::S3DVertex2TCoords* mb_vertices=(video::S3DVertex2TCoords*)mb->getVertices();
			mb_indices = mb->getIndices();
			numVertices = mb->getVertexCount();
			numIndices = mb->getIndexCount();

			for(j=0;j<numIndices;j+=3)
			{
				for (k=0;k<3;k++)
				{
					index = mb_indices[j+k];
					vertices[k] = btVector3(mb_vertices[index].Pos.X*scaling.X, mb_vertices[index].Pos.Y*scaling.Y, mb_vertices[index].Pos.Z*scaling.Z);
				}
				pTriMesh->addTriangle(vertices[0], vertices[1], vertices[2]);
			}
		}
	}
	return pTriMesh;
};