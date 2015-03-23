#ifndef __SPOTSHADOWMAPRENDERER_HPP__
#define __SPOTSHADOWMAPRENDERER_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Renders a shadow map in a single direction. Mainly intended for spot light sources.
 */

#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "ILightSceneNode.h"
#include "ITexture.h"
#include "matrix4.h"
#include "vector2d.h"

#include "..\\include\\Common.hpp"
#include "..\\include\\BaseShadowMapRenderer.hpp"

class SpotShadowMapRenderer : public BaseShadowMapRenderer
{
public:
	SpotShadowMapRenderer(irr::video::IVideoDriver *videoDriver, MaterialLibrary &materialLib, float margin = 0.025f, float znear = 0.05f, float zfar = 100.0f);
	virtual ~SpotShadowMapRenderer(void);

	virtual void setSceneManager(irr::scene::ISceneManager *sceneManager) override;
	virtual void renderShadows(irr::scene::ILightSceneNode *lightNode, ShadowMapAllocator &alloc);

	// Only valid after a call to renderShadows().
	virtual irr::video::ITexture* getRenderedTexture() { return m_ShadowTexture; }
	// Only valid after a call to renderShadows().
	virtual const irr::core::matrix4& getViewProjectionMatrix() { return m_ViewProjMatrix; }

	// Get X/Y offsets for sampling from the texture
	virtual irr::core::vector2df getTexOffset() { return m_TexOffset; }
	// Get X/Y scales for sampling from the texture
	virtual irr::core::vector2df getTexScale() { return m_TexScale; }

protected:
	ShadowRenderCallback m_RenderCallback;

	float m_Margin;
	// Projection matrix parameters
	irr::f32 m_ZNear, m_ZFar, m_OuterAngle;

	irr::video::ITexture *m_ShadowTexture;
	irr::core::matrix4 m_ViewProjMatrix;

	// For shaders to sample from the shadow map
	irr::core::vector2df m_TexOffset, m_TexScale;
};

#endif