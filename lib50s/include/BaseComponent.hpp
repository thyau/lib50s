#ifndef __BASECOMPONENT_HPP__
#define __BASECOMPONENT_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * An empty component that provides virtual stubs
 */

#include "..\\include\\IComponent.hpp"
#include "..\\include\\ComponentSpawnData.hpp"

/******************************************************
* Class  : BaseComponent (ABSTRACT)
* Purpose: A basic empty component, provides stubs for all methods others may wish to avoid implementing
* Notes  :
******************************************************/
class BaseComponent : public IComponent
{
	friend class ComponentFactory;

public:
							BaseComponent		();
	virtual					~BaseComponent		();

	// Public Methods
	virtual inline bool		allowMultiple		() const override { return false; }

	void					setNext				(const IComponent::Ptr &other) override { m_Next = other; }
	void					setPrevious			(const IComponent::Ptr &other) override { m_Previous = other; }
	inline IComponent::Ptr& getNext				() override { return m_Next; }
	inline IComponent::Ptr& getPrevious			() override { return m_Previous; }
	void					dropAllSiblings		() override;

	virtual ComponentSpawnData* getSpawnData	() { return m_SpawnData; }

	virtual void			update				(float delta) override STUB;

	// These signal methods are only stubs, so child components do not have to implement
	// signals they do not use and will not subscribe to
	virtual void			sig_AttachOther		(const IComponent::Ptr &other) override STUB;
	virtual void			sig_DetachOther		(const IComponent::Ptr &other) override STUB;
	//virtual void			sig_Update			(float delta) override STUB;
	virtual void			sig_PhysUpdate		(float delta) override STUB;
	virtual void			sig_Moved			(const vector3df& pos ) override STUB;
	virtual void			sig_Rotated			(const vector3df& rot ) override STUB;
	virtual void			sig_SceneChange		(const std::shared_ptr<Scene> &oldScene, const std::shared_ptr<Scene> &newScene) override STUB;
	virtual void			sig_ParentChange	(const std::shared_ptr<Entity> &oldParent, const std::shared_ptr<Entity> &newParent) override STUB;
	virtual void			sig_AddChild		(ISceneNode* child) override STUB;
	virtual void			sig_RemoveChild		(ISceneNode* child) override STUB;

protected:
	// Protected Methods
	virtual void			onAttach			(const std::shared_ptr<Entity> &parent) override;
	virtual void			onDetach			(const std::shared_ptr<Entity> &parent) override;

	void					unsubAll			();

	// Protected Members
	std::shared_ptr<Entity>	m_Parent;	// Note: This will create an ownership cycle.  However,
										// however, the performance hit of reference counting every
										// time a Component needs to lock its parent pointer
										// will be too significant.  In order for an Entity to be
										// automatically deleted, all of its components must
										// must be dropped first

	boost::signals2::connection m_Connections[ENT_SIGNAL_COUNT];

	IComponent::Ptr			m_Next;
	IComponent::Ptr			m_Previous;

	// We own this memory. May be NULL if, e.g., during an actual game we delete this data to save memory
	ComponentSpawnData *m_SpawnData; 
};

#endif