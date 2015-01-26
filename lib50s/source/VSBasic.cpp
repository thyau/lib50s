/*
	VSBasic.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

#include "..\\include\\VSBasic.hpp"
#include "..\\include\\ShaderCommon.hpp"

std::string VSBasic::buildSource()
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "#version 130"
	CODE ""
	CODE "varying vec3 VEyeCoord;"
	CODE ""
	CODE "void main() {"
	CODE "    vec4 eyeCoord = gl_ModelViewMatrix * gl_Vertex;"
	CODE "    VEyeCoord = eyeCoord.xyz;"
	CODE "    gl_Position = gl_ProjectionMatrix * eyeCoord;"
	CODE "}"
	CODE_END
	return o.str();
}
