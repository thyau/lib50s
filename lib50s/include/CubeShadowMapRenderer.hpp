#ifndef __CUBESHADOWMAPRENDERER_HPP__
#define __CUBESHADOWMAPRENDERER_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Renders shadow maps along the six cube face directions.
 * Mainly intended for point light sources.
 */

#include "ILightSceneNode.h"
#include "ISceneManager.h"
#include "ITexture.h"
#include "IVideoDriver.h"
#include "irrTypes.h"
#include "matrix4.h"
#include "vector2d.h"

#include "..\\include\\Common.hpp"
#include "..\\include\\BaseShadowMapRenderer.hpp"

class CubeShadowMapRenderer : public BaseShadowMapRenderer
{
public:
	CubeShadowMapRenderer(irr::video::IVideoDriver *videoDriver, MaterialLibrary &materialLib, float margin = 0.03f, float znear = 0.05f, float zfar = 100.0f);
	virtual ~CubeShadowMapRenderer(void);

	virtual void setSceneManager(irr::scene::ISceneManager *sceneManager) override;
	virtual void renderShadows(irr::scene::ILightSceneNode *lightNode, ShadowMapAllocator &alloc);

	// Only valid after a call to renderShadows().
	virtual irr::video::ITexture* getRenderedTexture() { return m_ShadowTexture; }
	// Only valid after a call to renderShadows().
	virtual const std::vector<irr::core::matrix4>& getViewProjectionMatrices() { return m_ViewProjMatrices; }

	// Get X/Y offsets for sampling from the texture
	virtual const std::vector<irr::core::vector2df>& getTexOffsets() { return m_TexOffsets; }
	// Get X/Y scales for sampling from the texture
	virtual const std::vector<irr::core::vector2df>& getTexScales() { return m_TexScales; }

protected:
	ShadowRenderCallback m_RenderCallback;
	//matrix4 m_ProjectionMatrix;
	float m_Margin; // Margin around each of the cube faces, to prevent sampling errors near the edges. Normalized units.

	// Projection matrix parameters
	irr::f32 m_Fovy, m_ZNear, m_ZFar, m_AspectRatio;

	irr::video::ITexture *m_ShadowTexture;
	std::vector<irr::core::matrix4> m_ViewProjMatrices;

	// For shaders to sample from the shadow map
	std::vector<irr::core::vector2df> m_TexOffsets, m_TexScales;
};

#endif