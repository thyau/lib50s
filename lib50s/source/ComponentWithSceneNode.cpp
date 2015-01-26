/*
	ComponentWithSceneNode.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Jon Schiavo
*/

#include "..\\include\\ComponentWithSceneNode.hpp"
#include "..\\include\\Entity.hpp"

ComponentWithSceneNode::ComponentWithSceneNode() :
	m_Node(NULL)
{
}

ComponentWithSceneNode::~ComponentWithSceneNode()
{
	unsetNodeImpl();
}

void ComponentWithSceneNode::setPositionOffset(const vector3df &offset)
{
	if(m_Node)
		m_Node->setPosition(offset);
}

void ComponentWithSceneNode::setRotationOffset(const vector3df &offset)
{
	if(m_Node)
		m_Node->setRotation(offset);
}

vector3df ComponentWithSceneNode::getPositionOffset() const
{
	if(m_Node)
		return m_Node->getPosition();
	else
		return ZERO_VECTOR;
}

vector3df ComponentWithSceneNode::getAbsPosition() const
{
	if(m_Node)
		return m_Node->getAbsolutePosition();
	else
		return ZERO_VECTOR;
}

vector3df ComponentWithSceneNode::getRotationOffset() const
{
	if(m_Node)
		return m_Node->getRotation();
	else
		return ZERO_VECTOR;
}

void ComponentWithSceneNode::onAttach(const std::shared_ptr<Entity> &parent)
{
	BaseComponent::onAttach(parent);

	m_Connections[ENT_SIGNAL_REMCHILD] = parent->subscribeRemoveChild(WO_CALL_FIRST, boost::bind(&ComponentWithSceneNode::sig_RemoveChild, this, _1));
}

void ComponentWithSceneNode::sig_RemoveChild(ISceneNode* child)
{
	if (m_Node && (child == m_Node))
	{
		m_Node->drop();
		m_Node = NULL;
	}
}

ISceneNode* ComponentWithSceneNode::setNodeImpl(ISceneNode* node)
{
	ISceneNode* old = m_Node;

	unsetNodeImpl();

	m_Node = node;
	if(m_Node)
	{
		m_Node->grab();
		attachToParent();
	}

	return old;
}

void ComponentWithSceneNode::unsetNodeImpl()
{
	if(m_Node)
	{
		detachFromParent();
		m_Node->drop();
		m_Node = NULL;
	}
}

void ComponentWithSceneNode::attachToParent()
{
	if(!m_Node || !(bool)m_Parent)
		return;

	m_Parent->addChild(m_Node);
}

void ComponentWithSceneNode::detachFromParent()
{
	if(!m_Node)
		return;

	if(m_Parent)
		m_Parent->removeChild(m_Node);
}
