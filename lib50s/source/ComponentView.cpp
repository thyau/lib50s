/*
	ComponentView.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Jon Schiavo
*/

#include "..\\include\\ComponentView.hpp"
#include "..\\include\\Entity.hpp"

using namespace irr::io;
using namespace irr::scene;

ComponentView::ComponentView() :
	ComponentWithSceneNode()
{
}

ComponentView::~ComponentView()
{
}

void ComponentView::makeActive(ISceneManager* smgr)
{
	if(smgr)
		smgr->setActiveCamera(dynamic_cast<ICameraSceneNode*>(this->getNode()));
}

void ComponentView::onAttach(const std::shared_ptr<Entity> &parent)
{
	BaseComponent::onAttach(parent);

	attachToParent();
}

void ComponentView::onDetach(const std::shared_ptr<Entity> &parent)
{
	BaseComponent::onDetach(parent);

	detachFromParent();
}

void ComponentView::SpawnData::serializeAttributes(IAttributes* out, SAttributeReadWriteOptions* options) const
{
	//TODO implement
}
	
void ComponentView::SpawnData::deserializeAttributes(IAttributes* in, SAttributeReadWriteOptions* options)
{
	//TODO implement
}