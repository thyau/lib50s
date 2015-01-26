/*
	PSNormalMap.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

#include "..\\include\\ShaderCommon.hpp"
#include "..\\include\\PSNormalMap.hpp"

using namespace shader;

std::string	PSNormalMap::buildSource()
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "#version 130"
	CODE ""
	CODE "uniform sampler2D TexColor;"
	CODE "uniform sampler2D TexNormal;"
	CODE "uniform float SpecularLevel;"
	CODE "uniform float SpecularPower;"
	CODE ""
	CODE "varying vec3 VNormal;"
	CODE "varying vec3 VTangent;"
	CODE "varying vec3 VBinormal;"
	CODE "varying float VDepth;"
	CODE ""
	CODE "void main() {"
	CODE "    vec4 color = texture(TexColor, gl_TexCoord[0].xy);"
	CODE "    vec3 texNormal = normalize(texture(TexNormal, gl_TexCoord[0].xy).rgb * 2.0 - 1.0);"
	CODE "    vec3 normal = normalize(VNormal) * texNormal.b + normalize(VTangent) * texNormal.r + normalize(VBinormal) * texNormal.g;"
	CODE "    float depth = VDepth;"
	CODE      writeGBuffer("color", "normal", "SpecularLevel", "SpecularPower", "depth")
	CODE "}"
	CODE_END
	return o.str();
}

IShaderConstantSetCallBack* PSNormalMap::getDefaultCallbackObject()
{
	return new Callback();
}

void PSNormalMap::Callback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	PSDiffuse::Callback::OnSetConstants(services, userData);

	irr::s32 normalTexLocation = 1;
	services->setPixelShaderConstant("TexNormal", &normalTexLocation, 1);
}