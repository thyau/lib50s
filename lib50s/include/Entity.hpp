#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Entity represents any object found in the world
 * Entities are a container for Components, which give them a
 * variety of properties.  Components can subscribe to signals
 * from their parent entity in order to be notified of certain
 * events
 */

#include <memory>

#include "ESceneNodeTypes.h"
#include "IAttributes.h"
#include "ISceneNode.h"
#include "ISceneManager.h"
#include "irrTypes.h"
#include "vector3d.h"

#include "..\\include\\Common.hpp"
#include "..\\include\\EntitySignal.hpp"
#include "..\\include\\IComponent.hpp"
#include "..\\include\\ComponentPhysical.hpp"
#include "..\\include\\ComponentCollider.hpp"

class Scene;

// Our custom addition to the entity-type enum
#define ESNT_ENTITY static_cast<irr::scene::ESCENE_NODE_TYPE>(MAKE_IRR_ID('e','n','t','y'))

/******************************************************
* Class  : Entity
* Purpose: Represents all in-world objects
* Notes  :
******************************************************/
class Entity : public irr::scene::ISceneNode, public std::enable_shared_from_this<Entity>
{
	friend class ComponentPhysical; // Physical component must be able to set position
									// without triggering signal
public:
	// NOTE!! Entity is special, do NOT create a new shared_ptr to it, and do NOT use shared_from_this.
	// Use ONLY getSharedPointer() to obtain shared pointers.
	typedef std::shared_ptr<Entity> Ptr;
	typedef std::weak_ptr<Entity> wPtr;

	//typedef std::map<long, Entity::Ptr> EntChildMap;

	// Constructor and Destructor
						Entity				(ISceneNode* parent, irr::scene::ISceneManager* mgr, long serialNum, irr::s32 id=-1);
	virtual				~Entity				();

	// Public Methods
	bool				addComponent		(const IComponent::Ptr &comp);
	bool				removeComponent		(const IComponent::Ptr &comp);
	inline bool			hasComponent		(COMPONENT_TYPE type) { return (bool)m_Components[type]; }
	inline const IComponent::Ptr& getComponent (COMPONENT_TYPE type) { return m_Components[type]; }

	void				setScene			(const std::shared_ptr<Scene> &scene);
	inline std::weak_ptr<Scene> getScene	() { return m_Scene; }

	// Pass isPrefix=true to match all children whose names begin with str
	// C++11 allows us to efficiently return a std::vector via the move constructor
	virtual std::vector<ISceneNode*> getChildrenByName(const std::string &str, bool isPrefix=false);

	// Update functions called by scene
	void				update				(float delta);
	void				physicsUpdate		(float delta);

	// Overridden functions to send signals
	virtual void		addChild			(ISceneNode* child) override;
	virtual bool		removeChild			(ISceneNode* child) override;
	virtual void		removeAll			() override;
	virtual void		setPosition			(const irr::core::vector3df& newpos) override;
	virtual void		setRotation			(const irr::core::vector3df& rotation) override;
	virtual void		setParent			(ISceneNode* newParent) override;
	virtual void		OnAnimate			(irr::u32 timeMs) override;

	// Trigger SubtreeChange events in all Entity nodes in the subtree.
	// Should be called by the user after making structural changes to the scene node tree.
	//  - subtreeRoot is the root node of the subtree, whether the subtree is is being attached or detached.
	static void			fireSubtreeUpdates	(ISceneNode* subtreeRoot);

	// Implementation of abstract or other necessary functions in ISceneNode
	virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox (void) const override;
	virtual void render						(void) override;
	virtual void OnRegisterSceneNode		(void) override;
	virtual void serializeAttributes		(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options=0) const;
	virtual void deserializeAttributes		(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options=0);

	// Misc functions
	//virtual void				setRotation		(const btQuaternion& rot);
	virtual irr::scene::ESCENE_NODE_TYPE	getType			() const override { return ESNT_ENTITY; }
	long									getSerialNum	(void) const { return m_SerialNum; }
	virtual void							setVisible		(bool isVisible) override;

	// Compute the absolute position of this entity immediately. Note that this might be a little bit
	// expensive and shouldn't be done at every frame, since the absolute position is already
	// computed in ISceneNode::OnAnimate(). This function is for convenience outside of the rendering loop.
	virtual void				updateAbsolutePositionRecursive(void);

	//TODO: Do we need these functions, or will it suffice to use getChildren() from ISceneNode?
	//Entity::Ptr			getChild			(long id);
	//bool				removeChild			(long id);

	// Use ISceneNode::removeAll() instead
	// void				removeAllChildren	();



	Entity::Ptr					getSharedPointer(void);

	boost::signals2::connection subscribeAttach (WO_CALL_ORDER order, AttachCallback bound);
	boost::signals2::connection subscribeDetach (WO_CALL_ORDER order, DetachCallback bound);
	//boost::signals2::connection subscribeUpdate (WO_CALL_ORDER order, UpdateCallback bound);
	boost::signals2::connection subscribePhysUpdate (WO_CALL_ORDER order, PhysUpdateCallback bound);
	boost::signals2::connection subscribeMoved (WO_CALL_ORDER order, MovedCallback bound);
	boost::signals2::connection subscribeRotated (WO_CALL_ORDER order, RotatedCallback bound);
	boost::signals2::connection subscribeSceneChange (WO_CALL_ORDER order, SceneChangeCallback bound);
	boost::signals2::connection subscribeParentChange (WO_CALL_ORDER order, ParentChangeCallback bound);
	boost::signals2::connection subscribeAddChild (WO_CALL_ORDER order, AddChildCallback bound);
	boost::signals2::connection subscribeRemoveChild (WO_CALL_ORDER order, RemoveChildCallback bound);
	boost::signals2::connection subscribeSubtreeChange (WO_CALL_ORDER order, SubtreeChangeCallback bound);
	//boost::signals2::connection subscribePreAnimate (WO_CALL_ORDER order, PreAnimateCallback bound);
	//boost::signals2::connection subscribePostAnimate (WO_CALL_ORDER order, PostAnimateCallback bound);

protected:
	// Protected Methods

	// Set position/rotation without sending signals
	virtual void		setPositionQuiet	(const irr::core::vector3df& newpos);
	virtual void		setRotationQuiet	(const irr::core::vector3df& rotation);
	virtual void		setRotationQuiet	(const btQuaternion& rot);

	void				removeAllComponents ();

	std::weak_ptr<Scene> m_Scene; // To prevent a posession cycle, m_Scene must be a weak_ptr

	IComponent::Ptr		m_Components[COMPONENT_COUNT];

	irr::core::aabbox3d<irr::f32> m_BoundingBox;

	// Serial number is assigned by the EntityFactory on creation. It is used to uniquely
	// identify the Entity at runtime, but may change from one run to another, and therefore
	// is not saved as a node attribute.
	const long m_SerialNum;

	// Whether or not any shared pointer has been created yet for this object
	// see getSharedPointer()
	bool m_HasSharedPtr;

	// The whole set of shared pointers will count as one "grab()" from Irrlicht. When all
	// shared pointers are gone, we do a drop() so that Irrlicht will delete us if needed.
	friend class EntityDeleter;
	class EntityDeleter
	{
	public:
		void operator() (Entity *e)
		{
			e->m_HasSharedPtr = false;
			e->drop();
		}
	};

	// TODO: Do we need a map data structure for efficient child access?
	//EntChildMap			m_Children;

	AttachSignal		m_sigAttach;
	DetachSignal		m_sigDetach;
	//UpdateSignal		m_sigUpdate;
	PhysUpdateSignal	m_sigPhysUpdate;
	MovedSignal			m_sigMoved;
	RotatedSignal		m_sigRotated;
	SceneChangeSignal	m_sigSceneChange;
	ParentChangeSignal	m_sigParentChange;
	AddChildSignal		m_sigAddChild;
	RemoveChildSignal	m_sigRemoveChild;
	SubtreeChangeSignal	m_sigSubtreeChange;
	//PreAnimateSignal	m_sigPreAnimate;
	//PostAnimateSignal	m_sigPostAnimate;
};

#endif