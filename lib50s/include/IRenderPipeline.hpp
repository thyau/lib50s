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

#include "ISceneManager.h"
#include "ITexture.h"

#include "..\\include\\Common.hpp"
#include "..\\include\\MaterialLibrary.hpp"

class IRenderPipeline
{
public:
	typedef std::shared_ptr<IRenderPipeline> Ptr;
	typedef std::weak_ptr<IRenderPipeline> wPtr;

	virtual RESULT	init					() = 0;
	// Perform render
	virtual void	run						() = 0;
	// Set the scene that we want to render
	virtual void	setSceneManager			(irr::scene::ISceneManager *sceneManager) = 0;

	// Set final render target to be a texture. May not be NULL.
	virtual void	setFinalRenderTarget	(irr::video::ITexture *texture, bool clearBackBuffer = true, bool clearZBuffer = true) = 0;
	// Set final render target to one of the predefined targets
	virtual void	setFinalRenderTarget	(irr::video::E_RENDER_TARGET target, bool clearBackBuffer = true, bool clearZBuffer = true) = 0;

	// Get the library of materials used by this rendering pipeline
	virtual const MaterialLibrary& getMaterialLibrary() = 0;
};

#endif