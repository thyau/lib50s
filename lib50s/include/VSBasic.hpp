#ifndef __VSBASIC_HPP__
#define __VSBASIC_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Basic vertex shader that computes eye and projected coordinates.
 */

#include "..\\include\\BaseVertexShader.hpp"

class VSBasic : public BaseVertexShader
{
public:
	virtual std::string	buildSource() override;
};

#endif