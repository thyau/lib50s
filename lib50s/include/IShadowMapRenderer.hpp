#ifndef __ISHADOWMAPRENDERER_HPP__
#define __ISHADOWMAPRENDERER_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Interface for rendering the shadow map depth buffers.
 */

#include "ISceneManager.h"
#include "ILightSceneNode.h"

#include "..\\include\\Common.hpp"
#include "..\\include\\ShadowMapAllocator.hpp"

class IShadowMapRenderer
{
public:
	// Set the scene that we want to render. Must have exactly one active camera.
	virtual void	setSceneManager	(irr::scene::ISceneManager *sceneManager) = 0;

	// Render shadows for the given light and currently active camera in the scene manager.
	// Textures are obtained from the given ShadowMapAllocator.
	// Note: the node's absolute position must have been updated.
	// Note: access to the rendered shadows is delegated to subclasses, as the specific 
	// usage of textures and other representation may vary.
	virtual void	renderShadows	(irr::scene::ILightSceneNode *lightNode, ShadowMapAllocator &alloc) = 0;
};

#endif