#ifndef __VSTANGENT_HPP__
#define __VSTANGENT_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Vertex shader that interpolates the vertex tangent space (normal, tangent, binormal), 
 * depth, and one texture coordinate.
 *
 * Coordinate locations:
 *	gl_MultiTexCoord0 - texture UV
 *  gl_MultiTexCoord1 - tangent xyz
 *  gl_MultiTexCoord2 - binormal xyz
 */

#include "..\\include\\BaseVertexShader.hpp"

class VSTangent : public BaseVertexShader
{
public:
	virtual std::string	buildSource() override;
};

#endif