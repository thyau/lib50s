/*
	VSNormal.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

#include "..\\include\\VSNormal.hpp"
#include "..\\include\\ShaderCommon.hpp"

using namespace shader;

std::string VSNormal::buildSource()
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "#version 130"
	CODE ""
	CODE "varying vec3 VNormal;"
	CODE "varying float VDepth;"

	CODE "varying vec3 VEyeCoord;"
	CODE "varying vec3 VModelCoord;"

	CODE ""
	CODE "void main() {"
	//CODE      projectAndSaveDepth("gl_Vertex", "gl_Position", "VDepth")

	CODE "    vec4 eyeCoord = gl_ModelViewMatrix * gl_Vertex;"
	CODE "    VEyeCoord = eyeCoord.xyz;"
	CODE "    VModelCoord = gl_Vertex.xyz;"
	CODE "    gl_Position = gl_ProjectionMatrix * eyeCoord;"

	CODE "    VNormal = normalize(gl_NormalMatrix * gl_Normal);"
	CODE "    gl_TexCoord[0] = gl_MultiTexCoord0;"
	CODE "}"
	CODE_END
	return o.str();
}
