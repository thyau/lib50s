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

#include "..\\include\\BasePixelShader.hpp"
#include "..\\include\\ShaderEffectRenderer.hpp"

class PSBoxBlur : public BasePixelShader
{
public:
	typedef struct {
		vector2df blurDirection;
		s32 numSamples;
		f32 stepSize;
		s32 inputTextureLoc;
	} BLUR_PARAMS;

	PSBoxBlur(const BLUR_PARAMS &params, const ShaderEffectRenderer *effectRenderer = NULL)
		: m_BlurParams(params), m_EffectRenderer(effectRenderer)
	{
	}

	virtual std::string	buildSource() override;
	IShaderConstantSetCallBack* getDefaultCallbackObject()
	{
		return new Callback(m_BlurParams, m_EffectRenderer);
	}

protected:

	class Callback : public IShaderConstantSetCallBack
	{
	public:
		Callback(const BLUR_PARAMS &params, const ShaderEffectRenderer *effectRenderer = NULL) 
			: m_BlurParams(params), m_EffectRenderer(effectRenderer) { }
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData) override;
	protected:
		const BLUR_PARAMS &m_BlurParams;
		const ShaderEffectRenderer *m_EffectRenderer;
	};

	const BLUR_PARAMS &m_BlurParams;
	const ShaderEffectRenderer *m_EffectRenderer;
};

#endif