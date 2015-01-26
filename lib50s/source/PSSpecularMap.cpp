/*
	PSSpecularMap.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

#include "..\\include\\ShaderCommon.hpp"
#include "..\\include\\PSSpecularMap.hpp"

using namespace shader;

std::string	PSSpecularMap::buildSource()
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "#version 130"
	CODE ""
	CODE "uniform sampler2D TexColor;"
	CODE "uniform sampler2D TexNormal;"
	CODE "uniform sampler2D TexSpecular;"
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

	CODE "    vec2 texSpecular = texture(TexSpecular, gl_TexCoord[0].xy).xy;"
	CODE "    float modulatedSpecLevel = texSpecular.x * SpecularLevel;"
	CODE "    float modulatedSpecPower = texSpecular.y * SpecularPower;"
	CODE      writeGBuffer("color", "normal", "modulatedSpecLevel", "modulatedSpecPower", "depth")
	CODE "}"
	CODE_END
	return o.str();
}

IShaderConstantSetCallBack* PSSpecularMap::getDefaultCallbackObject()
{
	return new Callback();
}

void PSSpecularMap::Callback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	PSNormalMap::Callback::OnSetConstants(services, userData);

	irr::s32 normalTexLocation = 2;
	services->setPixelShaderConstant("TexSpecular", &normalTexLocation, 1);
}