#ifndef __IRENDERPIPELINE_HPP__
#define __IRENDERPIPELINE_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * This interface represents an object that has the ability to render
 * a scene in some way.
 */

#include "..\\include\\Common.hpp"
#include "..\\include\\MaterialLibrary.hpp"

class IRenderPipeline
{
public:
	typedef std::shared_ptr<IRenderPipeline> Ptr;
	typedef std::weak_ptr<IRenderPipeline> wPtr;

	virtual RESULT	init					() PURE;
	// Perform render
	virtual void	run						() PURE;
	// Set the scene that we want to render
	virtual void	setSceneManager			(ISceneManager *sceneManager) PURE;

	// Set final render target to be a texture. May not be NULL.
	virtual void	setFinalRenderTarget	(ITexture *texture, bool clearBackBuffer = true, bool clearZBuffer = true) PURE;
	// Set final render target to one of the predefined targets
	virtual void	setFinalRenderTarget	(E_RENDER_TARGET target, bool clearBackBuffer = true, bool clearZBuffer = true) PURE;

	// Get the library of materials used by this rendering pipeline
	virtual const MaterialLibrary& getMaterialLibrary() PURE;
};

#endif