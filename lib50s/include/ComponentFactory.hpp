#ifndef __COMPONENT_FACTORY__HPP
#define __COMPONENT_FACTORY_HPP

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Factory-pattern for creating Component objects
 */

#include <memory>

#include "..\\include\\Common.hpp"

#include "..\\include\\IComponent.hpp"
#include "..\\include\\ComponentView.hpp"
#include "..\\include\\ComponentPhysical.hpp"
#include "..\\include\\ComponentMesh.hpp"
#include "..\\include\\ComponentScript.hpp"
#include "..\\include\\ComponentCollider.hpp"
#include "..\\include\\ComponentSpawnData.hpp"

class ComponentFactory
{
public:
	ComponentFactory(irr::scene::ISceneManager* sm);
	virtual ~ComponentFactory(void);

	IComponent::Ptr		createComponent			(const ComponentSpawnData *data);

	void				setCopySpawnData		(bool copyData)						{ m_CopySpawnData = copyData; }
	bool				getCopySpawnData		(void) const						{ return m_CopySpawnData; }

	// Clean up entries in the shape cache that point to already-deleted physics shapes.
	// Use when there are dramatic changes in the scene, e.g. loading/unloading.
	void				cleanPhysicsShapeCache	(void);

private:
	IComponent::Ptr		createViewComponent		(const ComponentView::SpawnData* data);
	IComponent::Ptr		createPhysicalComponent	(const ComponentPhysical::SpawnData* data);
	IComponent::Ptr		createColliderComponent	(const ComponentCollider::SpawnData* data);
	IComponent::Ptr		createMeshComponent		(const ComponentMesh::SpawnData* data);
	IComponent::Ptr		createScriptComponent   (const ComponentScript::SpawnData* data);

	irr::scene::ISceneManager *m_SceneManager; // Not owning

	bool m_CopySpawnData; // Whether or not to save a copy of the spawn data in created component

	typedef std::map<std::string, std::weak_ptr<btCollisionShape>> ShapeMap;
	ShapeMap m_ShapeCache;
};

#endif