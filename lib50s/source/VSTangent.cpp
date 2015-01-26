/*
	VSTangent.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

#include "..\\include\\VSTangent.hpp"
#include "..\\include\\ShaderCommon.hpp"

using namespace shader;

std::string	VSTangent::buildSource()
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "#version 130"
	CODE ""
	CODE "varying vec3 VNormal;"
	CODE "varying vec3 VTangent;"
	CODE "varying vec3 VBinormal;"
	CODE "varying float VDepth;"
	CODE ""
	CODE "void main() {"
	CODE      projectAndSaveDepth("gl_Vertex", "gl_Position", "VDepth")
	CODE "    VNormal = normalize(gl_NormalMatrix * gl_Normal);"
	CODE "    VTangent = normalize(gl_NormalMatrix * gl_MultiTexCoord1.xyz);"
	CODE "    VBinormal = normalize(gl_NormalMatrix * gl_MultiTexCoord2.xyz);"
	CODE "    gl_TexCoord[0] = gl_MultiTexCoord0;"
	CODE "}"
	CODE_END
	return o.str();
}