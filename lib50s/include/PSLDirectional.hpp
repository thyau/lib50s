#ifndef __PSLDIRECTIONAL_HPP__
#define __PSLDIRECTIONAL_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Pixel shader for directional lights (parallel rays from infinite distance).
 * 
 * Texture locations:
 *	0 : reserved
 *  1 : reserved
 *  2 : reserved
 *	3 : cascaded shadow map (optional, see constructor)
 */

#include "..\\include\\BasePixelShader.hpp"
#include "..\\include\\ILightContext.hpp"
#include "..\\include\\CascadedShadowMapRenderer.hpp"

// The compiled shader will support up to this number of shadow map cascades.
// Any cascades beyond this number will not be rendered.
// Increasing this number will also increase the amount of memory used for shader uniforms
#define LIGHT_DIR_MAX_SHADOW_CASCADES (std::size_t)4

class PSLDirectional : public BasePixelShader
{
public:
	// shadowRenderer may be null, in which case the generated shader will not compute shadows.
	PSLDirectional(ILightContext &lightContext, CascadedShadowMapRenderer *shadowRenderer);

	virtual std::string	buildSource() override;
	virtual irr::video::IShaderConstantSetCallBack* getDefaultCallbackObject() override;

protected:

	class Callback : public irr::video::IShaderConstantSetCallBack
	{
	public:
		Callback(ILightContext &lightContext, CascadedShadowMapRenderer *shadowRenderer) : m_LightContext(lightContext), m_ShadowRenderer(shadowRenderer) {}
		virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData);

	protected:
		ILightContext &m_LightContext;
		CascadedShadowMapRenderer *m_ShadowRenderer;
	};

	ILightContext &m_LightContext;
	CascadedShadowMapRenderer *m_ShadowRenderer;
};

#endif
