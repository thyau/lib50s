/*
	PSDiffuse.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

#include "..\\include\\ShaderCommon.hpp"
#include "..\\include\\PSDiffuse.hpp"

using namespace shader;

std::string	PSDiffuse::buildSource()
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "#version 130"
	CODE ""
	CODE "uniform sampler2D TexColor;"
	CODE "uniform float SpecularLevel;"
	CODE "uniform float SpecularPower;"
	CODE ""
	CODE "varying vec3 VNormal;"
	CODE "varying float VDepth;"

	CODE "varying vec3 VEyeCoord;"
	CODE "varying vec3 VModelCoord;"

	CODE ""
	CODE "void main() {"
	CODE "    vec4 color = texture(TexColor, gl_TexCoord[0].xy);"

	//CODE "if (VEyeCoord.z > 2.0) { color = vec4(0); }"
	//CODE "if (VEyeCoord.x < -0.5) { color = vec4(0); }"
	//CODE "if (VModelCoord.x < 0) { color = vec4(0); }"

	CODE "    vec3 normal = normalize(VNormal);"

	//CODE "    float depth = VDepth;"
	CODE "    float depth = VEyeCoord.z;"

	CODE      writeGBuffer("color", "normal", "SpecularLevel", "SpecularPower", "depth")
	CODE "}"
	CODE_END
	return o.str();
}

IShaderConstantSetCallBack* PSDiffuse::getDefaultCallbackObject()
{
	return new Callback();
}

void PSDiffuse::Callback::OnSetMaterial(const SMaterial& material)
{
	m_SpecularLevel = (float)material.SpecularColor.getRed() / 255.0f;
	m_SpecularPower = material.Shininess;
}

void PSDiffuse::Callback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	irr::s32 colorTexLocation = 0;
	services->setPixelShaderConstant("TexColor", &colorTexLocation, 1);

	services->setPixelShaderConstant("SpecularLevel", &m_SpecularLevel, 1);
	services->setPixelShaderConstant("SpecularPower", &m_SpecularPower, 1);
}