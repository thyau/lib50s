#ifndef __PSNULL_HPP__
#define __PSNULL_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Pixel shader that does absolutely nothing.
 */

#include "..\\include\\PSNormalMap.hpp"

class PSNull : public BasePixelShader
{
public:
	virtual std::string	buildSource() { return "void main() { discard; }"; }
};

#endif