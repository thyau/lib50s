/*
	PSDepthMoments.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

#include "..\\include\\PSDepthMoments.hpp"
#include "..\\include\\ShaderCommon.hpp"

using namespace shader;

std::string	PSDepthMoments::buildSource()
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "#version 130"
	CODE ""
	CODE "void main() {"
	// Use gl_FragCoord.z because it is affected by OpenGL's Polygon Offset (slope-scale depth bias) setting
	// It's also already in [0,1] which is what we want
	CODE "    gl_FragColor = vec4(gl_FragCoord.z - " << getFPBias() << ", gl_FragCoord.z * gl_FragCoord.z, 0, 0);" 
	CODE "}"
	CODE_END
	return o.str();
}
