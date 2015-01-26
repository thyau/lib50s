/*
	ComponentMesh.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Jon Schiavo
*/

#include "..\\include\\ComponentMesh.hpp"
#include "..\\include\\Entity.hpp"

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

void ComponentMesh::SpawnData::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	//TODO implement
}
	
void ComponentMesh::SpawnData::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	//TODO implement
}
