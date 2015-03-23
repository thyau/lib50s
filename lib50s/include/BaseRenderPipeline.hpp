#ifndef __BASERENDERPIPELINE_HPP__
#define __BASERENDERPIPELINE_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Partial implementation of the IRenderPipeline interface to provide common
 * data members and methods.
 */

#include "ISceneManager.h"
#include "ITexture.h"
#include "IVideoDriver.h"

#include "..\\include\\IRenderPipeline.hpp"
#include "..\\include\\MaterialLibrary.hpp"

class BaseRenderPipeline : public IRenderPipeline
{
public:
	BaseRenderPipeline(irr::video::IVideoDriver *videoDriver);
	virtual ~BaseRenderPipeline(void);

	virtual void	setSceneManager			(irr::scene::ISceneManager *sceneManager) override;
	virtual void	setFinalRenderTarget	(irr::video::ITexture *texture, bool clearBackBuffer = true, bool clearZBuffer = true) override;
	virtual void	setFinalRenderTarget	(irr::video::E_RENDER_TARGET target, bool clearBackBuffer = true, bool clearZBuffer = true) override;

	virtual const MaterialLibrary& getMaterialLibrary () override { return m_MaterialLib; }

protected:
	virtual void	doSetFinalRenderTarget(void);
	virtual void	doSetFinalRenderTarget(bool clearBack, bool clearZ);

	irr::video::IVideoDriver *m_VideoDriver;
	irr::scene::ISceneManager *m_SceneManager;

	irr::video::E_RENDER_TARGET m_FinalRenderTargetPredef;
	irr::video::ITexture *m_FinalRenderTargetTexture;
	bool m_FinalRenderClearBack, m_FinalRenderClearZ;

	MaterialLibrary m_MaterialLib;
};

#endif