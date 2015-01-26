/*
	PSLDirectional.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

#include "..\\include\\PSLDirectional.hpp"
#include "..\\include\\ShaderCommon.hpp"
#include "SLight.h"

using namespace shader;

PSLDirectional::PSLDirectional(ILightContext &lightContext, CascadedShadowMapRenderer *shadowRenderer)
	: m_LightContext(lightContext), m_ShadowRenderer(shadowRenderer)
{
}

std::string PSLDirectional::buildSource()
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "#version 130"
	CODE ""
	CODE includeGBuffer()
	CODE ""
	CODE "uniform vec3 LightDir;"
	CODE "uniform vec3 LightColor;"
	CODE "uniform vec3 LightAmbient;"
	CODE ""
	CODE "varying vec3 VEyeCoord;"
	CODE ""
	CODE_END

	if (m_ShadowRenderer)
	{
		CODE_BEGIN(o)
		CODE "uniform sampler2D CascadedShadowMap;"
		CODE "uniform mat4 LightProjMatrices[" << LIGHT_DIR_MAX_SHADOW_CASCADES << "];"
		CODE "uniform vec2 ShadowMapOffsets[" << LIGHT_DIR_MAX_SHADOW_CASCADES << "];"
		CODE "uniform vec2 ShadowMapScales[" << LIGHT_DIR_MAX_SHADOW_CASCADES << "];"
		CODE "uniform int NumCascades;"
		CODE ""
		CODE "uniform vec3 CascadeDebugColors[] = { vec3(1, 0, 1), vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1) };"
		CODE includeChebyshevUpperBound()
		CODE_END
	}

	CODE_BEGIN(o)
	CODE "void main() {"
	CODE      readGBuffer("color", "normal", "specLevel", "specPower", "depth", "gl_FragCoord.xy")
	CODE "    "
	CODE "    vec3 L = -LightDir;"
	CODE "    vec3 V = normalize(-VEyeCoord);"
	CODE "    "
	CODE      blinnPhongBRDF("L", "V", "normal", "specLevel", "specPower", "diffuseCoeff", "specCoeff")
	CODE "    "
	CODE "    float shadowFactor = 1.0;"
	CODE_END

	if (m_ShadowRenderer)
	{
		CODE_BEGIN(o)
		CODE "    vec3 pixelPos = (VEyeCoord / VEyeCoord.z) * depth;" // Pixel position in eye coordinates
		CODE ""
		CODE "    vec4 lightProjPos;"
		CODE "    vec2 shadowTexCoord;"
		CODE "    for (int i = 0; i < NumCascades; i++)"
		CODE "    {"
					  // In orthographic projection, this gives normalized device coordinates (NDC) directly; no need
					  // to divide by w. Still need to map NDC to [0,1] range for texture lookup though.
		CODE "        lightProjPos = LightProjMatrices[i] * vec4(pixelPos, 1.0);"
		CODE ""
		CODE "        if (lightProjPos.x >= -1 && lightProjPos.x <= 1 && lightProjPos.y >= -1 && lightProjPos.y <= 1)"
		CODE "        {"
		//CODE "            color.rgb = color.rgb * 0.5 + CascadeDebugColors[i] * 0.5;"
		CODE "            "
		CODE "            lightProjPos = lightProjPos * 0.5 + 0.5;"
		CODE "            shadowTexCoord = lightProjPos.xy * ShadowMapScales[i] + ShadowMapOffsets[i];"
		CODE "            "
		CODE              vsmShadow("lightProjPos.z", "CascadedShadowMap", "shadowTexCoord", "shadowFactorTmp")
		CODE "            shadowFactor = shadowFactorTmp;"
		CODE "            break;"
		CODE "        }"
		CODE "    }"
		CODE_END
	}

	CODE_BEGIN(o)
	CODE "    gl_FragColor = vec4(shadowFactor * LightColor * (color.rgb * diffuseCoeff + vec3(specCoeff)) + LightAmbient * color.rgb, 0);"
	CODE "}"
	CODE_END
	return o.str();
}

IShaderConstantSetCallBack* PSLDirectional::getDefaultCallbackObject()
{
	return new Callback(m_LightContext, m_ShadowRenderer);
}

void PSLDirectional::Callback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	setGBufferPSConstants(services);
	IVideoDriver *driver = services->getVideoDriver();

	const SLight *lightData = m_LightContext.getLightData();
	vector3df lightDir;
	driver->getTransform(ETS_VIEW).rotateVect(lightDir, lightData->Direction);

	services->setPixelShaderConstant("LightDir", (irr::f32*)&lightDir, 3);
	services->setPixelShaderConstant("LightColor", (irr::f32*)&lightData->DiffuseColor, 3);
	services->setPixelShaderConstant("LightAmbient", (irr::f32*)&lightData->AmbientColor, 3);

	if (m_ShadowRenderer)
	{
		const std::vector<matrix4> &projMats = m_ShadowRenderer->getViewProjectionMatrices();
		const std::vector<vector2df> &texOffsets = m_ShadowRenderer->getTexOffsets();
		const std::vector<vector2df> &texScales = m_ShadowRenderer->getTexScales();

		s32 numCascades = (s32)min(projMats.size(), LIGHT_DIR_MAX_SHADOW_CASCADES);
		services->setPixelShaderConstant("NumCascades", &numCascades, 1);

		float projMatArray[16*LIGHT_DIR_MAX_SHADOW_CASCADES];
		float texOffsetsArray[2*LIGHT_DIR_MAX_SHADOW_CASCADES];
		float texScalesArray[2*LIGHT_DIR_MAX_SHADOW_CASCADES];

		for (int i = 0; i < numCascades; i++)
		{
			memcpy(&projMatArray[i*16], projMats[i].pointer(), 16*sizeof(float));
			memcpy(&texOffsetsArray[i*2], (irr::f32*)&texOffsets[i], 2*sizeof(float));
			memcpy(&texScalesArray[i*2], (irr::f32*)&texScales[i], 2*sizeof(float));
		}

		services->setPixelShaderConstant("LightProjMatrices[0]", projMatArray, 16*numCascades);
		services->setPixelShaderConstant("ShadowMapOffsets[0]", texOffsetsArray, 2*numCascades);
		services->setPixelShaderConstant("ShadowMapScales[0]", texScalesArray, 2*numCascades);

		irr::s32 shadowTexLocation = 3;
		services->setPixelShaderConstant("CascadedShadowMap", &shadowTexLocation, 1);
	}
}