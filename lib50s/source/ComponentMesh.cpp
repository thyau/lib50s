/*
	ComponentMesh.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Jon Schiavo
*/

#include "..\\include\\ComponentMesh.hpp"
#include "..\\include\\Entity.hpp"

using namespace irr::io;

ComponentMesh::ComponentMesh() :
	ComponentWithSceneNode()
{
}

ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::onAttach(const std::shared_ptr<Entity> &parent)
{
	BaseComponent::onAttach(parent);

	attachToParent();
}

void ComponentMesh::onDetach(const std::shared_ptr<Entity> &parent)
{
	BaseComponent::onDetach(parent);
	
	detachFromParent();
}

void ComponentMesh::SpawnData::serializeAttributes(IAttributes* out, SAttributeReadWriteOptions* options) const
{
	//TODO implement
}
	
void ComponentMesh::SpawnData::deserializeAttributes(IAttributes* in, SAttributeReadWriteOptions* options)
{
	//TODO implement
}
