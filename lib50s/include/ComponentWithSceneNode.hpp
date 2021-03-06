#ifndef __COMPONENTWITHSCENENODE_HPP__
#define __COMPONENTWITHSCENENODE_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Because several different types of components will need to have scene
 * nodes, there is no point duplicating the logic for attaching/detaching
 * from the parent Entity's node.  This class is abstract, and sits between
 * BaseComponent, and any component which needs to hold a scene node
 */

#include "..\\include\\BaseComponent.hpp"

/******************************************************
* Class  : ComponentWithSceneNode (ABSTRACT)
* Purpose: Wrapper around logic for attaching/detaching from parent's node
* Notes  :
******************************************************/
class ComponentWithSceneNode : public BaseComponent
{
public:
						ComponentWithSceneNode		();
	virtual				~ComponentWithSceneNode		();

	inline ISceneNode*	getNode						() const { return m_Node; }

	void				setPositionOffset			(const vector3df &offset);
	void				setRotationOffset			(const vector3df &offset);
	vector3df			getPositionOffset			() const;
	vector3df			getAbsPosition				() const;
	vector3df			getRotationOffset			() const;

protected:
	// Protected Methods
	virtual void		onAttach					(const std::shared_ptr<Entity> &parent) override;

	virtual void		sig_RemoveChild				(ISceneNode* child) override;

	// This class does not allow external agents to set the node, it
	// only provides utility methods for the child class to use, allowing
	// it to expose that functionality on its own terms
	ISceneNode*			setNodeImpl					(ISceneNode* node);
	void				unsetNodeImpl				();

	void				attachToParent				();
	void				detachFromParent			();

private:
	// Private Members
	ISceneNode*			m_Node;
};

#endif