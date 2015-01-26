#ifndef __ICOMPONENT_HPP__
#define __ICOMPONENT_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Defines an interface for all Components.
 * Components are used to give Entities various properties.
 *
 * Because objects can have multiple instances of the same type
 * there are a number of ways to group them
 */

#include "..\\include\\Common.hpp"
#include "..\\include\\EntitySignal.hpp"

class Scene;

enum COMPONENT_TYPE
{
	COMPONENT_VIEW			= 0x1,
	COMPONENT_PHYSICAL		= 0x2,
	COMPONENT_MESH			= 0x3,
	COMPONENT_SCRIPT        = 0x4,
	COMPONENT_COLLIDER      = 0x5,

	COMPONENT_COUNT
};

/******************************************************
* Class  : IComponent (ABSTRACT)
* Purpose: Interface for all Entity components
* Notes  :
******************************************************/
class IComponent
{
	friend class Entity; // Entity must call several private methods, but no one else should

public:
	typedef std::shared_ptr<IComponent> Ptr;
	typedef std::weak_ptr<IComponent> wPtr;

	// Public Methods
	virtual inline bool allowMultiple() const PURE;
	virtual inline COMPONENT_TYPE getType() const PURE;

	virtual void setNext(const IComponent::Ptr& n) PURE;
	virtual void setPrevious(const IComponent::Ptr& p) PURE;
	virtual inline IComponent::Ptr& getNext() PURE;
	virtual inline IComponent::Ptr& getPrevious() PURE;
	virtual void dropAllSiblings() PURE;

	virtual void update(float delta) PURE;

	// Signal Handlers
	virtual void sig_AttachOther(const IComponent::Ptr &other) PURE;
	virtual void sig_DetachOther(const IComponent::Ptr &other) PURE;
	//virtual void sig_Update(float delta) PURE;
	virtual void sig_PhysUpdate(float delta) PURE;
	virtual void sig_Moved(const vector3df& pos ) PURE;
	virtual void sig_Rotated(const vector3df& rot ) PURE;
	virtual void sig_SceneChange(const std::shared_ptr<Scene> &oldScene, const std::shared_ptr<Scene> &newScene) PURE;
	virtual void sig_ParentChange(const std::shared_ptr<Entity> &oldParent, const std::shared_ptr<Entity> &newParent) PURE;
	virtual void sig_AddChild(ISceneNode* child) PURE;
	virtual void sig_RemoveChild(ISceneNode* child) PURE;

protected:
	// Protected Methods
	virtual void onAttach(const std::shared_ptr<Entity> &parent) PURE;
	virtual void onDetach(const std::shared_ptr<Entity> &parent) PURE;
};

#endif