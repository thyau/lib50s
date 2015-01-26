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

#include "..\\include\\IRenderPipeline.hpp"
#include "..\\include\\MaterialLibrary.hpp"

class BaseRenderPipeline : public IRenderPipeline
{
public:
	BaseRenderPipeline(IVideoDriver *videoDriver);
	virtual ~BaseRenderPipeline(void);

	virtual void	setSceneManager			(ISceneManager *sceneManager) override;
	virtual void	setFinalRenderTarget	(ITexture *texture, bool clearBackBuffer = true, bool clearZBuffer = true) override;
	virtual void	setFinalRenderTarget	(E_RENDER_TARGET target, bool clearBackBuffer = true, bool clearZBuffer = true) override;

	virtual const MaterialLibrary& getMaterialLibrary () override { return m_MaterialLib; }

protected:
	virtual void	doSetFinalRenderTarget(void);
	virtual void	doSetFinalRenderTarget(bool clearBack, bool clearZ);

	IVideoDriver *m_VideoDriver;
	ISceneManager *m_SceneManager;

	E_RENDER_TARGET m_FinalRenderTargetPredef;
	ITexture *m_FinalRenderTargetTexture;
	bool m_FinalRenderClearBack, m_FinalRenderClearZ;

	MaterialLibrary m_MaterialLib;
};

#endif