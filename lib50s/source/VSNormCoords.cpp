/*
	VSNormCoords.cpp
	Copyright (c) [ORGNAME] 2014
	Author: Timothy Yau
*/

#include "..\\include\\VSNormCoords.hpp"
#include "..\\include\\ShaderCommon.hpp"

std::string	VSNormCoords::buildSource()
{
	ostringstream o;
	CODE_BEGIN(o)
	CODE "#version 130"
	CODE "" 
	CODE "void main()"
	CODE "{"
	//CODE "    gl_Position = vec4(0);"
	CODE "    gl_Position = vec4(gl_Vertex.xyz, 1.0);"
	CODE "    gl_TexCoord[0] = gl_MultiTexCoord0;"
	CODE "}"
	CODE_END
	return o.str();
}