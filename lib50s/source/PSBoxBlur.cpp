/*
	PSBoxBlur.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

#include "..\\include\\PSBoxBlur.hpp"
#include "..\\include\\ShaderCommon.hpp"

std::string	PSBoxBlur::buildSource()
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "#version 130"
	CODE ""
	CODE "uniform vec2 BlurDirection;"
	CODE "uniform float StepSize;"
	CODE "uniform int NumSamples;"
	CODE "uniform sampler2D InputTex;"
	CODE_END

	if (m_EffectRenderer)
	{
		CODE_BEGIN(o)
		CODE "uniform vec2 TexOffset, TexScale;"
		CODE_END
	}

	CODE_BEGIN(o)
	CODE ""
	CODE "void main() {"
	//CODE "    gl_FragColor = vec4(0);"

	CODE "    vec2 sampleOffset = StepSize * BlurDirection;"
	CODE "    vec2 startCoord = gl_TexCoord[0].xy;"
	CODE_END

	if (m_EffectRenderer)
	{
		CODE_BEGIN(o)
		CODE "    startCoord = startCoord * TexScale + TexOffset;"
		CODE_END
	}

	CODE_BEGIN(o)
	CODE "    startCoord = startCoord - 0.5 * float(NumSamples - 1) * sampleOffset;"
	CODE "    vec4 sum = vec4(0);"
	CODE "    float numSamplesInv = 1 / float(NumSamples);"
	CODE "    "
	CODE "    for (int i = 0; i < NumSamples; i++)"
	CODE "    {"
	CODE "        sum += texture(InputTex, startCoord + i * sampleOffset);"
	CODE "    }"
	CODE "    "
	CODE "    gl_FragColor = sum * numSamplesInv;"
	CODE "}"
	CODE_END
	return o.str();
}

void PSBoxBlur::Callback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	services->setPixelShaderConstant("BlurDirection", (irr::f32*)&m_BlurParams.blurDirection, 2);
	services->setPixelShaderConstant("StepSize", &m_BlurParams.stepSize, 1);
	services->setPixelShaderConstant("NumSamples", &m_BlurParams.numSamples, 1);
	services->setPixelShaderConstant("InputTex", &m_BlurParams.inputTextureLoc, 1);

	if (m_EffectRenderer)
	{
		vector2df texOffset(0, 0), texScale(1, 1);
		m_EffectRenderer->getInputRegion(texOffset, texScale);
		services->setPixelShaderConstant("TexOffset", (irr::f32*)&texOffset, 2);
		services->setPixelShaderConstant("TexScale", (irr::f32*)&texScale, 2);
	}
}