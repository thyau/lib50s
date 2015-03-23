/*
	ShaderEffectRenderer.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

#include <utility>

#include "..\\include\\ShaderEffectRenderer.hpp"

using namespace irr;
using namespace irr::core;
using namespace irr::video;

ShaderEffectRenderer::ShaderEffectRenderer(IVideoDriver *driver)
	: m_VideoDriver(NULL), m_TexIn(NULL), m_TexOut(NULL), m_ViewportIn(0, 0, -1, -1), m_ViewportOut(0, 0, -1, -1),
	m_ClearBack(true), m_ClearZ(true)
{
	setVideoDriver(driver);

	m_Vertices[0] = S3DVertex(-1.0f,-1.0f,0.0f,0,0,1,SColor(0x0),0.0f,0.0f);
	m_Vertices[1] = S3DVertex(-1.0f, 1.0f,0.0f,0,0,1,SColor(0x0),0.0f,1.0f);
	m_Vertices[2] = S3DVertex( 1.0f, 1.0f,0.0f,0,0,1,SColor(0x0),1.0f,1.0f);
	m_Vertices[3] = S3DVertex( 1.0f,-1.0f,0.0f,0,0,1,SColor(0x0),1.0f,0.0f);
	m_Vertices[4] = S3DVertex(-1.0f,-1.0f,0.0f,0,0,1,SColor(0x0),0.0f,0.0f);
	m_Vertices[5] = S3DVertex( 1.0f, 1.0f,0.0f,0,0,1,SColor(0x0),1.0f,1.0f);

	m_Material.Lighting = false;
	m_Material.ZWriteEnable = false;
	m_Material.Wireframe = false;

	//m_Material.TextureLayer->AnisotropicFilter = false;
	//m_Material.TextureLayer->BilinearFilter = false;
	//m_Material.TextureLayer->TrilinearFilter = false;
}

ShaderEffectRenderer::~ShaderEffectRenderer()
{
	m_VideoDriver->drop();
}

void ShaderEffectRenderer::setDirectRender(ITexture *targetTexture, recti viewport)
{
	m_TexIn = NULL;
	m_ViewportIn = recti(0, 0, -1, -1);
	m_TexOut = targetTexture;
	m_ViewportOut = viewport;
}

void ShaderEffectRenderer::setPingPongRender(ITexture *targetTexture1, ITexture *targetTexture2, recti viewport1, recti viewport2)
{
	m_TexIn = targetTexture1;
	m_ViewportIn = viewport1;
	m_TexOut = targetTexture2;
	m_ViewportOut = viewport2;
}

void ShaderEffectRenderer::render(E_MATERIAL_TYPE materialType, u32 texLayer)
{
	irr::u16 indices[6] = {0, 1, 2, 3, 4, 5};

	m_Material.MaterialType = materialType;

	if (m_TexIn)
		m_Material.setTexture(texLayer, m_TexIn);

	m_VideoDriver->setMaterial(m_Material);
	m_VideoDriver->setRenderTarget(m_TexOut, m_ClearBack, m_ClearZ);
	if (m_ViewportOut.UpperLeftCorner.X <= m_ViewportOut.LowerRightCorner.X)
	{
		m_VideoDriver->setViewPort(recti(0, 0, 1, 1)); // HACK/WORKAROUND: Irrlicht bug when setting the same viewport twice
		m_VideoDriver->setViewPort(m_ViewportOut);
	}
	// else { TODO: do we need to set viewport to the entire texture, or does setRenderTarget do that? }
	m_VideoDriver->drawIndexedTriangleList(&m_Vertices[0], 6, &indices[0], 2);

	if (m_TexIn)
	{
		std::swap(m_TexIn, m_TexOut);
		std::swap(m_ViewportIn, m_ViewportOut);
	}
}

ITexture* ShaderEffectRenderer::getInputTexture() const
{
	return m_TexIn;
}

ITexture* ShaderEffectRenderer::getOutputTexture() const
{
	return m_TexOut;
}

ITexture* ShaderEffectRenderer::getLastRenderTexture() const
{
	return m_TexIn? m_TexIn : m_TexOut;
}

void ShaderEffectRenderer::setVideoDriver(IVideoDriver *driver)
{
	if (m_VideoDriver)
		m_VideoDriver->drop();

	m_VideoDriver = driver; 

	if (m_VideoDriver)
		m_VideoDriver->grab(); 
}

void ShaderEffectRenderer::getInputRegion(vector2df &offset, vector2df &scale) const
{
	if (m_TexIn)
	{
		if (m_ViewportIn.UpperLeftCorner.X > m_ViewportOut.LowerRightCorner.X)
		{
			offset.X = offset.Y = 0;
			scale.X = scale.Y = 1;
		}
		else
		{
			dimension2du texSize = m_TexIn->getSize();
			offset.X = (float)m_ViewportIn.UpperLeftCorner.X / (float)texSize.Width;
			offset.Y = (float)(texSize.Height - m_ViewportIn.LowerRightCorner.Y) / (float)texSize.Height;
			scale.X = (float)m_ViewportIn.getWidth() / (float)texSize.Width;
			scale.Y = (float)m_ViewportIn.getHeight() / (float)texSize.Height;
		}
	}
}