#include "..\include\ForwardRenderPipeline.hpp"

/*
	ForwardRenderPipeline.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

using namespace irr::video;

ForwardRenderPipeline::ForwardRenderPipeline(IVideoDriver *videoDriver) : BaseRenderPipeline(videoDriver)
{
}

ForwardRenderPipeline::~ForwardRenderPipeline(void)
{
}

RESULT ForwardRenderPipeline::init()
{
	m_MaterialLib.addIrrlichtBuiltInMaterials();
	return R_OK;
}

void ForwardRenderPipeline::run()
{
	doSetFinalRenderTarget();

	if (m_SceneManager)
		m_SceneManager->drawAll();
}