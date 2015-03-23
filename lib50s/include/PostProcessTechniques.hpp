#ifndef __POSTPROCESSTECHNIQUES_HPP__
#define __POSTPROCESSTECHNIQUES_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Collection of post-processing technique implementations.
 */

#include "EMaterialTypes.h"
#include "dimension2d.h"
#include "vector2d.h"

#include "..\\include\\ShaderInclude.hpp"
#include "..\\include\\ShaderEffectRenderer.hpp"

class PostProcessTechniques
{
public:
	// params is a reference to the param struct passed to the shader when it was constructed.
	// effectRenderer must have two textures set to ping-pong.
	// pixelScale allows sampling within pixels. For example, setting to 0.5 means that blurWidth
	// is interpreted in terms of half pixels.
	static void pixelBoxBlur(PSBoxBlur::BLUR_PARAMS &params, ShaderEffectRenderer &effectRenderer, irr::video::E_MATERIAL_TYPE boxBlurMaterial, 
		int blurWidth, float pixelScale = 1.0f)
	{
		irr::core::dimension2du textureSize = effectRenderer.getInputTexture()->getSize();
		irr::core::vector2df texelSize(pixelScale / (float)textureSize.Width, pixelScale / (float)textureSize.Height);

		params.inputTextureLoc = 0;
		params.numSamples = blurWidth;

		// Vertical blur
		params.blurDirection = irr::core::vector2df(0, 1);
		params.stepSize = texelSize.Y;
		effectRenderer.render(boxBlurMaterial, 0);

		// Horizontal blur
		params.blurDirection = irr::core::vector2df(1, 0);
		params.stepSize = texelSize.X;
		effectRenderer.render(boxBlurMaterial, 0);
	}
};

#endif