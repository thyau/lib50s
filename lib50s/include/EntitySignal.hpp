#ifndef __ENTITYSIGNAL_HPP__
#define __ENTITYSIGNAL_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Utility header for Entity signals
 */

#include <boost/signals2/signal.hpp>
#include <boost/signals2/shared_connection_block.hpp>
#include <boost/bind/bind.hpp>

class IComponent;
class Scene;
class Entity;

// --------------
// Enum describing signal types
enum ENT_SIGNAL_TYPE
{
	ENT_SIGNAL_NEWCOMP			= 0x0,	// New component added to parent object
	ENT_SIGNAL_REMCOMP			= 0x1,	// Component removed from parent object
	//ENT_SIGNAL_UPDATE			= 0x2,	// Need to do processing on frame update
	ENT_SIGNAL_PHYSUPDATE		= 0x3,  // Need to know every physics frame (can be different from render frame)
	ENT_SIGNAL_MOVED			= 0x4,	// The object has been forcibly (non-physically) moved
	ENT_SIGNAL_ROTATED			= 0x5,	// The object has been forcibly (non-physically) rotated
	ENT_SIGNAL_COLLIDE			= 0x6,	// The object has collided with another object
	ENT_SIGNAL_INPUT			= 0x7,	// The object has recieved some input from the player/AI
	ENT_SIGNAL_SCENECHANGE		= 0x8,	// The object has changed scenes
	ENT_SIGNAL_PARENTCHANGE		= 0x9,	// The parent object has changed
	ENT_SIGNAL_NEWCHILD			= 0xA,	// A new child Entity has been added
	ENT_SIGNAL_REMCHILD			= 0xB,	// A child has been removed
	ENT_SIGNAL_SUBTREE			= 0xC,	// The object tree has been changed in an unspecified way
	//ENT_SIGNAL_PRE_ANIMATE		= 0xD,  // Just before calling ISceneNode::OnAnimate (see source for details)
	//ENT_SIGNAL_POST_ANIMATE		= 0xE,  // Just after; absolute position of this node and all its children have been updated. DO NOT change node position any more.

	ENT_SIGNAL_COUNT
};

// --------------
// Enum for signal calling order
enum WO_CALL_ORDER
{
	WO_CALL_FIRST = 0,
	WO_CALL_SECOND = 1,
	WO_CALL_THIRD = 3,
	WO_CALL_FOURTH = 4,
};

// --------------
// Signal result combiner function
// Takes a series of returns and OR's them together
template<typename T>
struct SIGNAL_COMBINER_OR
{
	typedef T result_type;

	template<typename InputIterator>
	T operator()(InputIterator first, InputIterator last) const
	{
		T out = *first++;
		while(first != last)
		{
			out |= (T)(*first);
			++first;
		}
		return out;
	}
};

typedef boost::signals2::signal<void (const std::shared_ptr<IComponent>&)> AttachSignal; // Adding component
typedef boost::signals2::signal<void (const std::shared_ptr<IComponent>&)> DetachSignal; // Removing component
//typedef boost::signals2::signal<void (float)> UpdateSignal;
typedef boost::signals2::signal<void (float)> PhysUpdateSignal;
typedef boost::signals2::signal<void (const vector3df&)> MovedSignal;
typedef boost::signals2::signal<void (const vector3df&)> RotatedSignal;
typedef boost::signals2::signal<void (const std::shared_ptr<Scene>&, const std::shared_ptr<Scene>&)> SceneChangeSignal;
typedef boost::signals2::signal<void (ISceneNode*, ISceneNode*)> ParentChangeSignal;
typedef boost::signals2::signal<void (ISceneNode* child)> AddChildSignal; // Just before child added
typedef boost::signals2::signal<void (ISceneNode* child)> RemoveChildSignal; // Just after child removed
typedef boost::signals2::signal<void (ISceneNode* subtreeRoot)> SubtreeChangeSignal;
//typedef boost::signals2::signal<void (u32 timeMs)> PreAnimateSignal;
//typedef boost::signals2::signal<void (u32 timeMs)> PostAnimateSignal;

typedef boost::function<void (const std::shared_ptr<IComponent>&)> AttachCallback;
typedef boost::function<void (const std::shared_ptr<IComponent>&)> DetachCallback;
//typedef boost::function<void (float)> UpdateCallback;
typedef boost::function<void (float)> PhysUpdateCallback;
typedef boost::function<void (const vector3df&)> MovedCallback;
typedef boost::function<void (const vector3df&)> RotatedCallback;
typedef boost::function<void (const std::shared_ptr<Scene>&, const std::shared_ptr<Scene>&)> SceneChangeCallback;
typedef boost::function<void (ISceneNode* /* Old parent */, ISceneNode* /* New parent */)> ParentChangeCallback;
typedef boost::function<void (ISceneNode* child)> AddChildCallback;
typedef boost::function<void (ISceneNode* child)> RemoveChildCallback;
typedef boost::function<void (ISceneNode* subtreeRoot)> SubtreeChangeCallback;
//typedef boost::function<void (u32 timeMs)> PreAnimateCallback;
//typedef boost::function<void (u32 timeMs)> PostAnimateCallback;

#endif