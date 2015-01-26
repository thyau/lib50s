#ifndef __VSNULL_HPP__
#define __VSNULL_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Vertex shader that does absolutely nothing.
 */

#include "..\\include\\BaseVertexShader.hpp"

class VSNull : public BaseVertexShader
{
public:
	virtual std::string	buildSource() {
		return "void main() { gl_Position = vec4(0); }";
	}
};

#endif