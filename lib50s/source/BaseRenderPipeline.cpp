#include "..\include\BaseRenderPipeline.hpp"

/*
	BaseRenderPipeline.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

BaseRenderPipeline::BaseRenderPipeline(IVideoDriver *videoDriver) 
	: m_VideoDriver(videoDriver), m_SceneManager(NULL),
	m_FinalRenderTargetPredef(ERT_FRAME_BUFFER), m_FinalRenderTargetTexture(NULL),
	m_FinalRenderClearBack(true), m_FinalRenderClearZ(true),
	m_MaterialLib(videoDriver)
{
	m_VideoDriver->grab();
}


BaseRenderPipeline::~BaseRenderPipeline(void)
{
	if (m_SceneManager)
		m_SceneManager->drop();

	if (m_FinalRenderTargetTexture)
		m_FinalRenderTargetTexture->drop();

	m_VideoDriver->drop();
}

void BaseRenderPipeline::setSceneManager(ISceneManager *sceneManager)
{
	if (m_SceneManager)
		m_SceneManager->drop();

	m_SceneManager = sceneManager;

	if (m_SceneManager)
		m_SceneManager->grab();
}

void BaseRenderPipeline::setFinalRenderTarget(ITexture *texture, bool clearBackBuffer, bool clearZBuffer)
{
	if (m_FinalRenderTargetTexture)
		m_FinalRenderTargetTexture->drop();

	// We won't allow setting texture to NULL, which means to set to previous target, because the 
	// previous target could be anything depending on the rendering procedure.
	assert(texture && "BaseRenderPipeline::setFinalRenderTarget: setting render target texture to NULL is ambiguous.");

	m_FinalRenderClearBack = clearBackBuffer;
	m_FinalRenderClearZ = clearZBuffer;
	m_FinalRenderTargetTexture = texture;

	m_FinalRenderTargetTexture->grab();
}

void BaseRenderPipeline::setFinalRenderTarget(E_RENDER_TARGET target, bool clearBackBuffer, bool clearZBuffer)
{
	m_FinalRenderClearBack = clearBackBuffer;
	m_FinalRenderClearZ = clearZBuffer;
	m_FinalRenderTargetPredef = target;

	if (m_FinalRenderTargetTexture)
	{
		m_FinalRenderTargetTexture->drop();
		m_FinalRenderTargetTexture = NULL;
	}
}

void BaseRenderPipeline::doSetFinalRenderTarget()
{
	doSetFinalRenderTarget(m_FinalRenderClearBack, m_FinalRenderClearZ);
}

void BaseRenderPipeline::doSetFinalRenderTarget(bool clearBack, bool clearZ)
{
	if (m_FinalRenderTargetTexture)
	{
		m_VideoDriver->setRenderTarget(m_FinalRenderTargetTexture, clearBack, clearZ);
	}
	else
	{
		m_VideoDriver->setRenderTarget(m_FinalRenderTargetPredef, clearBack, clearZ);
	}
}