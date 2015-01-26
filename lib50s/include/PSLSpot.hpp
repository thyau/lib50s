#ifndef __PSLSPOT_HPP__
#define __PSLSPOT_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Pixel shader for spot (cone) lights.
 * In addition to the light in the cone, ambient (non-directional) light can also be
 * emitted by setting the AmbientColor of the light.
 * 
 * Texture locations:
 *	0 : reserved
 *  1 : reserved
 *  2 : reserved
 *	3 : shadow map (optional, see constructor)
 */

#include "..\\include\\BasePixelShader.hpp"
#include "..\\include\\ILightContext.hpp"
#include "..\\include\\SpotShadowMapRenderer.hpp"

class PSLSpot : public BasePixelShader
{
public:
	// shadowRenderer may be null, in which case the generated shader will not compute shadows.
	PSLSpot(ILightContext &lightContext, SpotShadowMapRenderer *shadowRenderer);

	virtual std::string	buildSource() override;
	virtual IShaderConstantSetCallBack* getDefaultCallbackObject() override;

protected:

	class Callback : public IShaderConstantSetCallBack
	{
	public:
		Callback(ILightContext &lightContext, SpotShadowMapRenderer *shadowRenderer) : m_LightContext(lightContext), m_ShadowRenderer(shadowRenderer) {}
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

	protected:
		ILightContext &m_LightContext;
		SpotShadowMapRenderer *m_ShadowRenderer;
	};

	ILightContext &m_LightContext;
	SpotShadowMapRenderer *m_ShadowRenderer;
};

#endif
