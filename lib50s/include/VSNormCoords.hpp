#ifndef __VSNORMCOORDS_HPP__
#define __VSNORMCOORDS_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Vertex shader that writes vertex coordinates directly to normalized device coordinates.
 * Recall that in normalized device coordinates, the x, y, and z values are all in [-1,1].
 * Useful for things like rendering a screen-filling quad.
 */

#include "..\\include\\BaseVertexShader.hpp"

class VSNormCoords : public BaseVertexShader
{
public:
	virtual std::string	buildSource();
};

#endif