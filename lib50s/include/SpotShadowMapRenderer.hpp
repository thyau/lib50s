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

#include "..\\include\\Common.hpp"
#include "..\\include\\BaseShadowMapRenderer.hpp"

class SpotShadowMapRenderer : public BaseShadowMapRenderer
{
public:
	SpotShadowMapRenderer(IVideoDriver *videoDriver, MaterialLibrary &materialLib, float margin = 0.025f, float znear = 0.05f, float zfar = 100.0f);
	virtual ~SpotShadowMapRenderer(void);

	virtual void setSceneManager(ISceneManager *sceneManager) override;
	virtual void renderShadows(ILightSceneNode *lightNode, ShadowMapAllocator &alloc);

	// Only valid after a call to renderShadows().
	virtual ITexture* getRenderedTexture() { return m_ShadowTexture; }
	// Only valid after a call to renderShadows().
	virtual const matrix4& getViewProjectionMatrix() { return m_ViewProjMatrix; }

	// Get X/Y offsets for sampling from the texture
	virtual vector2df getTexOffset() { return m_TexOffset; }
	// Get X/Y scales for sampling from the texture
	virtual vector2df getTexScale() { return m_TexScale; }

protected:
	ShadowRenderCallback m_RenderCallback;

	float m_Margin;
	// Projection matrix parameters
	f32 m_ZNear, m_ZFar, m_OuterAngle;

	ITexture *m_ShadowTexture;
	matrix4 m_ViewProjMatrix;

	// For shaders to sample from the shadow map
	vector2df m_TexOffset, m_TexScale;
};

#endif