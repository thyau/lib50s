#ifndef __DEBUG_SCENE_HPP__
#define __DEBUG_SCENE_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Manages a separate scene to display debug objects, and synchronizes
 * them with the actual scene.
 */

#include <memory>

#include "ISceneManager.h"
#include "ISceneNode.h"
#include "IGeometryCreator.h"
#include "IMeshManipulator.h"
#include "IMeshSceneNode.h"

#include "Common.hpp"
#include "Entity.hpp"
#include "ComponentPhysical.hpp"

class DebugScene : public std::enable_shared_from_this<DebugScene>
{
public:
	typedef std::shared_ptr<DebugScene> Ptr;
	typedef std::weak_ptr<DebugScene> wPtr;

	DebugScene(irr::scene::ISceneManager *sceneMgr, irr::scene::ISceneManager *debugSceneMgr);
	virtual ~DebugScene(void);

	// Sync debug objects with tracked nodes
	void	updateScene			(void); // Remember to check reference counts

	// Start tracking the given node with debug objects. If there are no more
	// references to the node and it is about to be destroyed, tracking stops
	// automatically.
	void	trackNode			(irr::scene::ISceneNode *node); // Remember to grab()

	// Track all nodes in the subtree
	void	trackNodeSubtree	(irr::scene::ISceneNode *root);

	bool	stopTrackingNode	(irr::scene::ISceneNode *node);
	void	stopTrackingSubtree	(irr::scene::ISceneNode *root);
	void	stopTrackingAll		(void);

	void	setDrawBoundingBox	(bool newvalue) { m_DrawBoundingBox = newvalue; }
	bool	getDrawBoundingBox	(void) const	{ return m_DrawBoundingBox; }
	void	setDrawPhysicsShape	(bool newvalue) { m_DrawPhysicsShape = newvalue; }
	bool	getDrawPhysicsShape	(void) const	{ return m_DrawPhysicsShape; }

protected:
	bool m_DrawBoundingBox, m_DrawPhysicsShape;

	irr::scene::ISceneManager *m_SceneMgr, *m_DebugSceneMgr;
	const irr::scene::IGeometryCreator *m_GeomCreator;
	irr::scene::IMeshManipulator *m_MeshManipulator;

	typedef struct NODE_TRACKING_INFO
	{
		NODE_TRACKING_INFO(void) : m_TrackedNode(NULL), m_BoundingBoxNode(NULL) { }

		~NODE_TRACKING_INFO(void)
		{
			// Clean up nodes, except for m_TrackedNode
			if (m_BoundingBoxNode)
			{
				m_BoundingBoxNode->remove();
				m_BoundingBoxNode = NULL;
			}

			for (irr::scene::ISceneNode *node : m_PhysicsShapeNodes)
				node->remove();

			m_PhysicsShapeNodes.clear();
			m_PhysicsShapes.clear();
		}

		irr::scene::ISceneNode *m_TrackedNode;

		irr::scene::IMeshSceneNode *m_BoundingBoxNode;

		std::vector<irr::scene::IMeshSceneNode*> m_PhysicsShapeNodes;
		// The shapes represented by the physics shape nodes. Used to check
		// for changes during updateScene().
		std::vector<btCollisionShape*> m_PhysicsShapes;
	} NODE_TRACKING_INFO;

	std::vector<NODE_TRACKING_INFO> trackingInfo;

	irr::scene::IMeshSceneNode* createNodeForShape(btCollisionShape *shape) const;
	void			setWireframeMatProps(irr::scene::IMeshSceneNode *node) const;
	void			setMeshColor		(irr::scene::IMeshSceneNode *node, int lightID) const;

	void		updateBoundingBoxNode	(NODE_TRACKING_INFO &info);
	void		updatePhysicsNodes		(NODE_TRACKING_INFO &info);

	void		addCameraAndLights		(irr::scene::ISceneManager *debugMgr);
	void		removeCameraAndLights	(irr::scene::ISceneManager *debugMgr);
	void		updateCameraAndLightPos	(irr::scene::ISceneManager *mgr, irr::scene::ISceneManager *debugMgr);

	enum DEBUG_SPECIAL_OBJECT_ID
	{
		DEBUG_CAMERA_ID				= -2, // Negative to avoid potential clashing with actual IDs.
		DEBUG_LIGHT_BOUNDING_BOX	= -3, 
		DEBUG_LIGHT_PHYSICS_SHAPE	= -4
	};

	//// Custom light manager to allow us to light up wireframes with different colors
	//class DebugLightManager : public scene::ILightManager
	//{
	//	virtual void OnPreRender(core::array<ISceneNode*> & lightList) { m_SceneLightList = &lightList; }

	//	virtual void OnPostRender(void) { }

	//	virtual void OnRenderPassPreRender(E_SCENE_NODE_RENDER_PASS renderPass) { }

	//	virtual void OnRenderPassPostRender(E_SCENE_NODE_RENDER_PASS renderPass) { }

	//	virtual void OnNodePreRender(ISceneNode* node);

	//	virtual void OnNodePostRender(ISceneNode* node) { }

	//protected:
	//	core::array<scene::ISceneNode*> *m_SceneLightList;
	//};

	//DebugLightManager *m_LightManager;
};

#endif