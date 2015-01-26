#ifndef __VSNORMAL_HPP__
#define __VSNORMAL_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Simple vertex shader that computes vertex normal, depth, and one texture coordinate.
 *
 * Coordinate locations:
 *	gl_MultiTexCoord0 - texture UV
 */

#include "..\\include\\BaseVertexShader.hpp"

class VSNormal : public BaseVertexShader
{
public:
	virtual std::string	buildSource() override;
};

#endif