/*
	PSLPoint.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

#include "..\\include\\PSLPoint.hpp"
#include "..\\include\\ShaderCommon.hpp"
#include "vector3d.h"
#include "SLight.h"

using namespace irr;
using namespace irr::core;
using namespace irr::video;

using namespace shader;

PSLPoint::PSLPoint(ILightContext &lightContext, CubeShadowMapRenderer *shadowRenderer)
	: m_LightContext(lightContext), m_ShadowRenderer(shadowRenderer)
{
}

std::string PSLPoint::buildSource()
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "#version 130"
	CODE ""
	CODE includeGBuffer()
	CODE ""
	CODE "uniform vec3 LightPos;"
	CODE "uniform vec3 LightAtt;"
	CODE "uniform vec3 LightColor;"
	CODE ""
	CODE "varying vec3 VEyeCoord;"
	CODE ""
	CODE_END

	if (m_ShadowRenderer)
	{
		CODE_BEGIN(o)
		CODE "uniform sampler2D FlatCubeShadowMap;"
		CODE "uniform mat4 LightProjMatrices[6];"
		CODE "uniform vec2 ShadowMapOffsets[6], ShadowMapScales[6];"
		CODE "uniform mat4 ViewMatrixInverse;"
		CODE ""
		CODE includeChebyshevUpperBound()
		CODE ""
		CODE rescaleDistHeader()
		CODE_END
	}

	CODE_BEGIN(o)
	CODE "void main() {"
	CODE      readGBuffer("color", "normal", "specLevel", "specPower", "depth", "gl_FragCoord.xy")
	CODE "    "
	CODE "    vec3 pixelPos = (VEyeCoord / VEyeCoord.z) * depth;" // Pixel position in eye coordinates

	CODE "    vec3 lightVec = LightPos - pixelPos;"
	CODE "    float lightDist = length(lightVec);"
	CODE "    vec3 L = lightVec / lightDist;"
	CODE "    vec3 V = normalize(-pixelPos);"

	CODE "    float lightAttFactor = " << lightAttenuationFactor("LightAtt", "lightDist")
	CODE      blinnPhongBRDF("L", "V", "normal", "specLevel", "specPower", "diffuseCoeff", "specCoeff")
	CODE "    "
	CODE_END

	if (m_ShadowRenderer)
	{
		CODE_BEGIN(o)
		CODE "int cubeIdx;"
		CODE "vec3 P = (ViewMatrixInverse * vec4(-L, 0)).xyz;"
		CODE "float maxDir = max(abs(P.x), max(abs(P.y), abs(P.z)));"
		CODE "if (maxDir == abs(P.x))"
		CODE "{"
		CODE "    if (P.x > 0)"
		CODE "        cubeIdx = 0;"
		CODE "    else"
		CODE "        cubeIdx = 3;"
		CODE "}"
		CODE "else if (maxDir == abs(P.y))"
		CODE "{"
		CODE "    if (P.y > 0)"
		CODE "        cubeIdx = 1;"
		CODE "    else"
		CODE "        cubeIdx = 4;"
		CODE "}"
		CODE "else"
		CODE "{"
		CODE "    if (P.z > 0)"
		CODE "        cubeIdx = 2;"
		CODE "    else"
		CODE "        cubeIdx = 5;"
		CODE "}"

		CODE "vec4 lightProjPos = LightProjMatrices[cubeIdx] * vec4(pixelPos, 1.0);" // Project point onto the light's perspective camera

		CODE "vec2 shadowTexCoord = (lightProjPos.xy / lightProjPos.w) * 0.5 + 0.5;" // Perspective division gives coords in range [-1,1], map to range [0,1] for texture lookup
		CODE "shadowTexCoord.x = shadowTexCoord.x * ShadowMapScales[cubeIdx].x + ShadowMapOffsets[cubeIdx].x;"
		CODE "shadowTexCoord.y = shadowTexCoord.y * ShadowMapScales[cubeIdx].y + ShadowMapOffsets[cubeIdx].y;"

		CODE "float lightDistRescaled = lightDist;"
		CODE rescaleDist("lightDistRescaled")
		CODE vsmShadow("lightDistRescaled", "FlatCubeShadowMap", "shadowTexCoord", "shadowFactor")
		
		CODE "lightAttFactor *= shadowFactor;"
		CODE_END
	}

	CODE_BEGIN(o)
	CODE "    gl_FragColor = vec4(LightColor * lightAttFactor * (color.rgb * diffuseCoeff + vec3(specCoeff)), 0);"
	CODE "}"
	CODE_END
	return o.str();
}

IShaderConstantSetCallBack* PSLPoint::getDefaultCallbackObject()
{
	return new Callback(m_LightContext, m_ShadowRenderer);
}

void PSLPoint::Callback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	setGBufferPSConstants(services);
	IVideoDriver *driver = services->getVideoDriver();

	const SLight *lightData = m_LightContext.getLightData();
	const vector3df &lightPos = m_LightContext.getLightEyePos();

	services->setPixelShaderConstant("LightPos", (irr::f32*)&lightPos, 3);
	services->setPixelShaderConstant("LightAtt", (irr::f32*)&lightData->Attenuation, 3);
	services->setPixelShaderConstant("LightColor", (irr::f32*)&lightData->DiffuseColor, 3);

	if (m_ShadowRenderer)
	{
		const std::vector<matrix4> &projMats = m_ShadowRenderer->getViewProjectionMatrices();
		const std::vector<vector2df> &texOffsets = m_ShadowRenderer->getTexOffsets();
		const std::vector<vector2df> &texScales = m_ShadowRenderer->getTexScales();

		float projMatArray[16*6];
		float texOffsetsArray[2*6];
		float texScalesArray[2*6];

		for (int i = 0; i < 6; i++)
		{
			memcpy(&projMatArray[i*16], projMats[i].pointer(), 16*sizeof(float));
			memcpy(&texOffsetsArray[i*2], (irr::f32*)&texOffsets[i], 2*sizeof(float));
			memcpy(&texScalesArray[i*2], (irr::f32*)&texScales[i], 2*sizeof(float));
		}

		// Looks like using, e.g., LightProjMatrices[2] doesn't work
		// So we flatten all the data into linear arrays 
		services->setPixelShaderConstant("LightProjMatrices[0]", projMatArray, 16*6);
		services->setPixelShaderConstant("ShadowMapOffsets[0]", texOffsetsArray, 2*6);
		services->setPixelShaderConstant("ShadowMapScales[0]", texScalesArray, 2*6);

		irr::s32 shadowTexLocation = 3;
		services->setPixelShaderConstant("FlatCubeShadowMap", &shadowTexLocation, 1);

		services->setPixelShaderConstant("ViewMatrixInverse", m_LightContext.getRenderViewMatrixInverse().pointer(), 16);

		rescaleDistSetConstants(services, m_ShadowRenderer->getCameraContext());
	}
}
