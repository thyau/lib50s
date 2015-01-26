/*
	BaseComponent.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Jon Schiavo
*/

#include "..\\include\\BaseComponent.hpp"
#include "..\\include\\Entity.hpp"

BaseComponent::BaseComponent() 
	: m_SpawnData(NULL)
{
}

BaseComponent::~BaseComponent()
{
	unsubAll();
	SAFE_DELETE(m_SpawnData);
}

void BaseComponent::dropAllSiblings()
{
	if((bool)m_Next)
	{
		// Clear the previous pointer, have the next child drop its next (and down the line), then drop the next
		m_Next->setPrevious(IComponent::Ptr());
		m_Next->dropAllSiblings();
		m_Next.reset();
	}
}

void BaseComponent::onAttach(const std::shared_ptr<Entity> &parent)
{
	// Assert that we have been removed before being attached
	assert(!(bool)m_Parent && "BaseComponent::onAttach: Cannot be attached to an Entity before being removed from previous Entity");
	m_Parent = parent;
	cout<<this<<" attached"<<endl;

	m_Connections[ENT_SIGNAL_NEWCOMP] = m_Parent->subscribeAttach(WO_CALL_FIRST, boost::bind(&BaseComponent::sig_AttachOther, this, _1));
	m_Connections[ENT_SIGNAL_REMCOMP] = m_Parent->subscribeDetach(WO_CALL_FIRST, boost::bind(&BaseComponent::sig_DetachOther, this, _1));
}

void BaseComponent::onDetach(const std::shared_ptr<Entity> &parent)
{
	assert(parent == m_Parent && "BaseComponent::onDetach: Attempted to call BaseComponent::onDetach with wrong parent pointer");
	unsubAll();
	m_Parent.reset();
	cout<<this<<" detached"<<endl;
}

void BaseComponent::unsubAll()
{
	for(long i = 0; i < ENT_SIGNAL_COUNT; ++i)
		m_Connections[i].disconnect();
}