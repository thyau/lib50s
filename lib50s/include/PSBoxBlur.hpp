#ifndef __PSBOXBLUR_HPP__
#define __PSBOXBLUR_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Box blur effect.
 */

#include "irrTypes.h"
#include "IMaterialRendererServices.h"
#include "IShaderConstantSetCallBack.h"
#include "vector2d.h"

#include "..\\include\\BasePixelShader.hpp"
#include "..\\include\\ShaderEffectRenderer.hpp"

class PSBoxBlur : public BasePixelShader
{
public:
	typedef struct {
		irr::core::vector2df blurDirection;
		irr::s32 numSamples;
		irr::f32 stepSize;
		irr::s32 inputTextureLoc;
	} BLUR_PARAMS;

	PSBoxBlur(const BLUR_PARAMS &params, const ShaderEffectRenderer *effectRenderer = NULL)
		: m_BlurParams(params), m_EffectRenderer(effectRenderer)
	{
	}

	virtual std::string	buildSource() override;
	irr::video::IShaderConstantSetCallBack* getDefaultCallbackObject()
	{
		return new Callback(m_BlurParams, m_EffectRenderer);
	}

protected:

	class Callback : public irr::video::IShaderConstantSetCallBack
	{
	public:
		Callback(const BLUR_PARAMS &params, const ShaderEffectRenderer *effectRenderer = NULL) 
			: m_BlurParams(params), m_EffectRenderer(effectRenderer) { }
		virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData) override;
	protected:
		const BLUR_PARAMS &m_BlurParams;
		const ShaderEffectRenderer *m_EffectRenderer;
	};

	const BLUR_PARAMS &m_BlurParams;
	const ShaderEffectRenderer *m_EffectRenderer;
};

#endif