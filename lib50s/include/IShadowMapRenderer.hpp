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

#include "..\\include\\Common.hpp"
#include "..\\include\\ShadowMapAllocator.hpp"

class IShadowMapRenderer
{
public:
	// Set the scene that we want to render. Must have exactly one active camera.
	virtual void	setSceneManager	(ISceneManager *sceneManager) PURE;

	// Render shadows for the given light and currently active camera in the scene manager.
	// Textures are obtained from the given ShadowMapAllocator.
	// Note: the node's absolute position must have been updated.
	// Note: access to the rendered shadows is delegated to subclasses, as the specific 
	// usage of textures and other representation may vary.
	virtual void	renderShadows	(ILightSceneNode *lightNode, ShadowMapAllocator &alloc) PURE;
};

#endif