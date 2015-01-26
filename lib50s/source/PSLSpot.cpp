/*
	PSLSpot.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

#include "..\\include\\PSLSpot.hpp"
#include "..\\include\\ShaderCommon.hpp"
#include "SLight.h"

using namespace shader;

PSLSpot::PSLSpot(ILightContext &lightContext, SpotShadowMapRenderer *shadowRenderer)
	: m_LightContext(lightContext), m_ShadowRenderer(shadowRenderer)
{
}

std::string PSLSpot::buildSource()
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
	CODE "uniform vec3 LightDir;"
	CODE "uniform vec3 LightAmbient;"
	CODE "uniform float CosInnerAngle;"
	CODE "uniform float CosOuterAngle;"
	CODE ""
	// Pixel position (eye coordinates) for the point on the screen quad (or other geometry)
	// being rendered to represent the screen coverage area of this light.
	CODE "varying vec3 VEyeCoord;"
	CODE ""
	CODE_END

	if (m_ShadowRenderer)
	{
		CODE_BEGIN(o)
		CODE "uniform sampler2D SpotShadowMap;"
		CODE "uniform mat4 LightProjMatrix;"
		CODE "uniform vec2 ShadowMapOffset, ShadowMapScale;"
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
	CODE      radialAttenuationFactor("L", "LightDir", "CosInnerAngle", "CosOuterAngle", "radialAttFactor")
	CODE_END

	if (m_ShadowRenderer)
	{
		CODE_BEGIN(o)
		CODE "vec4 lightProjPos = LightProjMatrix * vec4(pixelPos, 1.0);" // Project point onto the light's perspective camera

		CODE "vec2 shadowTexCoord = (lightProjPos.xy / lightProjPos.w) * 0.5 + 0.5;" // Perspective division gives coords in range [-1,1], map to range [0,1] for texture lookup
		CODE "shadowTexCoord.x = shadowTexCoord.x * ShadowMapScale.x + ShadowMapOffset.x;"
		CODE "shadowTexCoord.y = shadowTexCoord.y * ShadowMapScale.y + ShadowMapOffset.y;"

		CODE "float lightDistRescaled = lightDist;"
		CODE rescaleDist("lightDistRescaled")
		CODE vsmShadow("lightDistRescaled", "SpotShadowMap", "shadowTexCoord", "shadowFactor")
		
		// Shadow only affects the light within the cone (see output equation below)
		CODE "radialAttFactor *= shadowFactor;"

		//CODE "if (lightProjPos.x > 0) { lightAttFactor = 0; }"
		//CODE "if (lightProjPos.w > 1.0) { lightAttFactor = 0; }"
		//CODE "if (lightDist > 2.0) { lightAttFactor = 0; }"
		//CODE "if (shadowTexCoord.y > 0.5) { lightAttFactor = 0; }"
		//CODE "if (lightDist > texture(SpotShadowMap, shadowTexCoord).x) { lightAttFactor = 0; }"

		CODE_END
	}

	CODE_BEGIN(o)
	CODE "    gl_FragColor = vec4(lightAttFactor * LightColor * radialAttFactor * (color.rgb * diffuseCoeff + vec3(specCoeff)) + lightAttFactor * LightAmbient * color.rgb, 0);"
	CODE "}"
	CODE_END
	return o.str();
}

IShaderConstantSetCallBack* PSLSpot::getDefaultCallbackObject()
{
	return new Callback(m_LightContext, m_ShadowRenderer);
}

void PSLSpot::Callback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	setGBufferPSConstants(services);
	IVideoDriver *driver = services->getVideoDriver();

	const SLight *lightData = m_LightContext.getLightData();
	const vector3df &lightPos = m_LightContext.getLightEyePos();
	vector3df lightDir;
	driver->getTransform(ETS_VIEW).rotateVect(lightDir, lightData->Direction);

	// TODO: would it be faster to do this on GPU? Or avoid it altogether?
	f32 cosInnerAngle = std::cos(lightData->InnerCone);
	f32 cosOuterAngle = std::cos(lightData->OuterCone);

	services->setPixelShaderConstant("LightPos", (irr::f32*)&lightPos, 3);
	services->setPixelShaderConstant("LightAtt", (irr::f32*)&lightData->Attenuation, 3);
	services->setPixelShaderConstant("LightColor", (irr::f32*)&lightData->DiffuseColor, 3);
	services->setPixelShaderConstant("LightAmbient", (irr::f32*)&lightData->AmbientColor, 3);
	services->setPixelShaderConstant("LightDir", (irr::f32*)&lightDir, 3);
	//services->setPixelShaderConstant("LightDir", (irr::f32*)&lightData->Direction, 3);
	services->setPixelShaderConstant("CosInnerAngle", &cosInnerAngle, 1);
	services->setPixelShaderConstant("CosOuterAngle", &cosOuterAngle, 1);

	if (m_ShadowRenderer)
	{
		matrix4 projMat = m_ShadowRenderer->getViewProjectionMatrix();
		vector2df texOffset = m_ShadowRenderer->getTexOffset();
		vector2df texScale = m_ShadowRenderer->getTexScale();



		if (!services->setPixelShaderConstant("LightProjMatrix", projMat.pointer(), 16))
		{
			PRINT("Set lightprojmatrix failed" << endl, WHITE);
		}
		if (!services->setPixelShaderConstant("ShadowMapOffset", (irr::f32*)&texOffset, 2))
		{
			PRINT("Set ShadowMapOffset failed" << endl, WHITE);
		}
		if (!services->setPixelShaderConstant("ShadowMapScale", (irr::f32*)&texScale, 2))
		{
			PRINT("Set ShadowMapScale failed" << endl, WHITE);
		}

		irr::s32 shadowTexLocation = 3;
		if (!services->setPixelShaderConstant("SpotShadowMap", &shadowTexLocation, 1))
		{
			PRINT("Set SpotShadowMap failed" << endl, WHITE);
		}

		rescaleDistSetConstants(services, m_ShadowRenderer->getCameraContext());
	}
}
