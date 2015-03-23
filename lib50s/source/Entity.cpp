/*
	Entity.cpp
	Copyright (c) [ORGNAME] 2013
	Author: Jon Schiavo
*/

#include <stack>

#include "..\\include\\Entity.hpp"
#include "..\\include\\Scene.hpp"

using namespace std;

using namespace irr;
using namespace irr::core;
using namespace irr::scene;

Entity::Entity(ISceneNode* parent, ISceneManager* mgr, long serialNum, s32 id) : 
	ISceneNode(parent, mgr, id), m_BoundingBox(vector3df(0,0,0)), m_SerialNum(serialNum), m_HasSharedPtr(false)
{
	//setVisible(true);
}

Entity::~Entity()
{
	removeAllComponents();
}

bool Entity::addComponent(const IComponent::Ptr &comp)
{
	if(!(bool)comp)
		return false;

	COMPONENT_TYPE type = comp->getType();

	if((bool)m_Components[type])
	{
		// If a component of this type is already attached
		// we need to make sure we can have multiple instances of it.
		// If we can, add it to the end of the linked list.
		// If not, return fail
		if(comp->allowMultiple())
		{
			IComponent::Ptr c = m_Components[type];
			while((bool)c->getNext())
			{
				// If we find that this component is already attached
				// we don't want to attach it twice
				if(c == comp)
					return false;
				c = c->getNext();
			}

			// Link new element on
			c->setNext(comp);
			comp->setPrevious(c);
		}
		else
		{
			return false;
		}
	}
	else
	{
		// If no component already exists for this type, we can
		// just assign it
		m_Components[type] = comp;
	}

	// Tell everybody
	m_sigAttach(comp);

	// Tell the new component
	comp->onAttach(shared_from_this());

	return true;
}

bool Entity::removeComponent(const IComponent::Ptr &comp)
{
	if(!(bool)comp)
		return false;

	COMPONENT_TYPE type = comp->getType();

	// If we allow multiple components for this type, we need to check
	// all the way down the chain
	if(comp->allowMultiple())
	{
		IComponent::Ptr c = m_Components[type];
		while(((bool)c) && (c != comp) && (c = c->getNext()))
			; // Empty

		if(!(bool)c)
			return false;

		// We have to tell everyone first, because we may invalidate the object
		comp->onDetach(shared_from_this());
		m_sigDetach(comp);
		
		// Is this the first component?
		if(c == m_Components[type])
		{
			IComponent::Ptr next = c->getNext();
			if((bool)next)
				next->setPrevious(IComponent::Ptr());
			m_Components[type] = next;
		}
		else
		{
			IComponent::Ptr next, prev;
			next = c->getNext();
			prev = c->getPrevious();
			if((bool)next)
				next->setPrevious(prev);
			if((bool)prev)
				prev->setNext(next);
		}
	}
	else
	{
		// Otherwise, verify we are removing a component we actually have
		if(m_Components[type] != comp)
			return false;
		
		comp->onDetach(shared_from_this());
		m_sigDetach(comp);
		m_Components[type].reset();
	}

	return true;
}

void Entity::setScene(const Scene::Ptr &scene)
{
	// Assert that if the scene is currently set, the new one must be NULL
	// i.e. we are being removed from our old scene
	// Cannot set scene without removing from old scene
	assert((m_Scene.expired() || !(bool)scene) && "Entity::setScene: Cannot set new scene without first removing from old");

	m_sigSceneChange(m_Scene.lock(), scene);
	m_Scene = scene;
}

vector<ISceneNode*> Entity::getChildrenByName(const std::string &str, bool isPrefix)
{
	const core::list<ISceneNode*>& list = getChildren();
	vector<ISceneNode*> out;

	core::list<ISceneNode*>::ConstIterator it;
	for (it = list.begin(); it != list.end(); it++)
	{
		ISceneNode *node = *it;

		if ((!isPrefix && str.compare(node->getName())) ||
			(isPrefix && str.compare(0, std::string::npos, node->getName(), str.length())))
		{
			out.push_back(node);
		}
	}

	return out;
}

void Entity::addChild(ISceneNode* child)
{
	ISceneNode::addChild(child);
	m_sigAddChild(child);

	if (child->getType() == ESNT_ENTITY) {
		// Make sure that this entity is in the same scene as us
		// the scene will handle updating the object so we don't have to
		Entity* e = dynamic_cast<Entity*>(child);
		Scene::wPtr otherScene = e->getScene();
		if(otherScene._Get() != m_Scene._Get())
		{
			if(Scene::Ptr locked = otherScene.lock())
				locked->removeEntity(e->getID());

			if(Scene::Ptr locked = m_Scene.lock())
				locked->addEntity(e->shared_from_this()); // TODO not sure if this is right
		}
	}
}

bool Entity::removeChild(ISceneNode* child)
{
	// Ensure that child is not deleted before signal is sent
	child->grab(); 

	if(ISceneNode::removeChild(child))
	{
		m_sigRemoveChild(child);
		child->drop();
		return true;
	}
	else
		return false;
}

void Entity::removeAll()
{
	//TODO: Inefficient
	while (Children.size() > 0)
	{
		// Iterator is a temporary. Let's avoid invalidating iterators when calling
		// removeChild, which modifies the list.
		ISceneNode* child = *(Children.begin());
		removeChild(child);
	}
}

void Entity::setPosition(const vector3df &pos)
{
	setPositionQuiet(pos);
	m_sigMoved(pos);
}

void Entity::setRotation(const vector3df &rot)
{
	setRotationQuiet(rot);
	m_sigRotated(rot);
}

void Entity::update(float delta)
{
	//m_sigUpdate(delta);

	IComponent::Ptr c;

	for (int i = 0; i < COMPONENT_COUNT; i++)
	{
		c = m_Components[i];

		while (c)
		{
			c->update(delta);
			c = c->getNext();
		}
	}
}

void Entity::physicsUpdate(float delta)
{
	m_sigPhysUpdate(delta);
}

boost::signals2::connection Entity::subscribeAttach(WO_CALL_ORDER order, AttachCallback bound)
{
	return m_sigAttach.connect(order, bound);
}

boost::signals2::connection Entity::subscribeDetach(WO_CALL_ORDER order, AttachCallback bound)
{
	return m_sigDetach.connect(order, bound);
}

//boost::signals2::connection Entity::subscribeUpdate(WO_CALL_ORDER order, UpdateCallback bound)
//{
//	return m_sigUpdate.connect(order, bound);
//}

boost::signals2::connection Entity::subscribeMoved(WO_CALL_ORDER order, MovedCallback bound)
{
	return m_sigMoved.connect(order, bound);
}

boost::signals2::connection Entity::subscribeRotated(WO_CALL_ORDER order, RotatedCallback bound)
{
	return m_sigRotated.connect(order, bound);
}

boost::signals2::connection Entity::subscribeSceneChange(WO_CALL_ORDER order, SceneChangeCallback bound)
{
	return m_sigSceneChange.connect(order, bound);
}

boost::signals2::connection Entity::subscribeParentChange(WO_CALL_ORDER order, ParentChangeCallback bound)
{
	return m_sigParentChange.connect(order, bound);
}

boost::signals2::connection Entity::subscribeAddChild(WO_CALL_ORDER order, AddChildCallback bound)
{
	return m_sigAddChild.connect(order, bound);
}

boost::signals2::connection Entity::subscribeRemoveChild(WO_CALL_ORDER order, RemoveChildCallback bound)
{
	return m_sigRemoveChild.connect(order, bound);
}

boost::signals2::connection Entity::subscribeSubtreeChange(WO_CALL_ORDER order, SubtreeChangeCallback bound)
{
	return m_sigSubtreeChange.connect(order, bound);
}

//boost::signals2::connection Entity::subscribePreAnimate(WO_CALL_ORDER order, PreAnimateCallback bound)
//{
//	return m_sigPreAnimate.connect(order, bound);
//}

//boost::signals2::connection Entity::subscribePostAnimate (WO_CALL_ORDER order, PostAnimateCallback bound)
//{
//	return m_sigPostAnimate.connect(order, bound);
//}

void Entity::setPositionQuiet(const vector3df& newpos)
{
	ISceneNode::setPosition(newpos);
}

void Entity::setRotationQuiet(const vector3df& rotation)
{
	ISceneNode::setRotation(rotation);
}

void Entity::setRotationQuiet(const btQuaternion& rot)
{
	quaternion q(rot.getX(), rot.getY(), rot.getZ(), rot.getW());
	vector3df euler;
	q.toEuler(euler);
	euler *= RADTODEG;

	setRotationQuiet(euler);
}

void Entity::removeAllComponents()
{
	for(long i = 0; i < COMPONENT_COUNT; ++i)
	{
		// TODO inefficient, but easiest way to send all the signals
		while ((bool)m_Components[i])
		{
			removeComponent(m_Components[i]);
		}
	}
}

void Entity::updateAbsolutePositionRecursive(void)
{
	std::stack<ISceneNode*> nodeStack;

	ISceneNode *node = this;
	while (node)
	{
		nodeStack.push(node);
		node = node->getParent();
	}

	while (!nodeStack.empty())
	{
		nodeStack.top()->updateAbsolutePosition();
		nodeStack.pop();
	}
}

Entity::Ptr Entity::getSharedPointer()
{ 
	if (m_HasSharedPtr)
		return shared_from_this(); 
	else
	{
		m_HasSharedPtr = true;
		grab();
		return Entity::Ptr(this, EntityDeleter());
	}
}

void Entity::setParent(ISceneNode* newParent)
{
	// TIMOTHY: What double-update? Setting a new parent doesn't automatically register us
	// with the parent's scene
	//if((bool)Parent)
	//{
	//	// We must remove ourselves from the scene so we don't get double-updated
	//	if(Scene::Ptr scene = m_Scene.lock())
	//	{
	//		scene->removeEntity(getID());
	//	}
	//}

	m_sigParentChange(Parent, newParent);
	ISceneNode::setParent(newParent);
}

void Entity::OnAnimate(u32 timeMs)
{
	if (IsVisible)
	{
		//m_sigPreAnimate(timeMs);
		ISceneNode::OnAnimate(timeMs);
		//m_sigPostAnimate(timeMs);

		// Only one of each of these components is allowed!
		ComponentCollider::Ptr &cc = dynamic_pointer_cast<ComponentCollider>(m_Components[COMPONENT_COLLIDER]);
		if (cc) { cc->postAnimate(timeMs); }

		ComponentPhysical::Ptr &cp = dynamic_pointer_cast<ComponentPhysical>(m_Components[COMPONENT_PHYSICAL]);
		if (cp) { cp->postAnimate(timeMs); }
	}
}

void Entity::fireSubtreeUpdates(ISceneNode* subtreeRoot)
{
	// Depth-first traversal order to save memory on large scenes.
	// Trigger a signal on all Entity nodes.
	std::stack<ISceneNode*> nodeList;
	nodeList.push(subtreeRoot);

	while (!nodeList.empty())
	{
		ISceneNode* node = nodeList.top();
		nodeList.pop();

		const core::list<ISceneNode*>& children = node->getChildren();
		for (core::list<ISceneNode*>::ConstIterator it = children.begin();
			it != children.end(); it++)
		{
			nodeList.push(*it);
		}

		if (node->getType() == ESNT_ENTITY)
		{
			Entity *e = dynamic_cast<Entity*>(node);
			e->m_sigSubtreeChange(subtreeRoot);
		}
	}
}

const aabbox3d<f32>& Entity::getBoundingBox() const
{
	return m_BoundingBox;
}

void Entity::render()
{
	// No-op
	// Scene node components are our child nodes and should be automatically rendered
}

void Entity::OnRegisterSceneNode()
{
	if (IsVisible)
	{
		SceneManager->registerNodeForRendering(this);
	}

    ISceneNode::OnRegisterSceneNode();
}

void Entity::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	ISceneNode::serializeAttributes(out, options);

	// TODO Add any other attributes that we need
	// Note that components are handled by EntityDataSerializer
}


void Entity::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	ISceneNode::deserializeAttributes(in, options);

	//TODO Add any other attributes that we need
}

void Entity::setVisible(bool isVisible)
{
	ISceneNode::setVisible(isVisible);

	if (m_Components[COMPONENT_PHYSICAL])
	{
		ComponentPhysical::Ptr cp = dynamic_pointer_cast<ComponentPhysical>(m_Components[COMPONENT_PHYSICAL]);
		cp->setEnabled(isVisible);
	}
}

