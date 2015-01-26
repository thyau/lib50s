/*
	PSDepth.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

#include "..\\include\\PSDepth.hpp"
#include "..\\include\\ShaderCommon.hpp"

std::string	PSDepth::buildSource()
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "#version 130"
	CODE ""
	CODE "void main() {"
	// Use gl_FragCoord.z because it is affected by OpenGL's Polygon Offset (slope-scale depth bias) setting
	CODE "    float depthNDC = gl_FragCoord.z * 2.0 - 1.0;" // gl_FragCoord.z is in the range [0,1], convert it to normalized device coordinates
	CODE "    float depthEye = gl_ProjectionMatrix[3][2] / (depthNDC - gl_ProjectionMatrix[2][2]);" // Convert NDC back to eye space
	CODE "    gl_FragColor = vec4(depthEye, 0, 0, 0);" 
	CODE "}"
	CODE_END
	return o.str();
}
