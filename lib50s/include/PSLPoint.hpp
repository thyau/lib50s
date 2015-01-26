#ifndef __PSLPOINT_HPP__
#define __PSLPOINT_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Pixel shader for omni-directional point lights.
 * 
 * Texture locations:
 *	0 : reserved
 *  1 : reserved
 *  2 : reserved
 *	3 : cube shadow map (optional, see constructor)
 */

#include "..\\include\\BasePixelShader.hpp"
#include "..\\include\\ILightContext.hpp"
#include "..\\include\\CubeShadowMapRenderer.hpp"

class PSLPoint : public BasePixelShader
{
public:
	// shadowRenderer may be null, in which case the generated shader will not compute shadows.
	PSLPoint(ILightContext &lightContext, CubeShadowMapRenderer *shadowRenderer);

	virtual std::string	buildSource() override;
	virtual IShaderConstantSetCallBack* getDefaultCallbackObject() override;

protected:

	class Callback : public IShaderConstantSetCallBack
	{
	public:
		Callback(ILightContext &lightContext, CubeShadowMapRenderer *shadowRenderer) : m_LightContext(lightContext), m_ShadowRenderer(shadowRenderer) {}
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

	protected:
		ILightContext &m_LightContext;
		CubeShadowMapRenderer *m_ShadowRenderer;
	};

	ILightContext &m_LightContext;
	CubeShadowMapRenderer *m_ShadowRenderer;
};

#endif
