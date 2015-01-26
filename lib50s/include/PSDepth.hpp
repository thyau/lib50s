#ifndef __PSDEPTH_HPP__
#define __PSDEPTH_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Renders eye-space depth.
 */

#include "..\\include\\BasePixelShader.hpp"

class PSDepth : public BasePixelShader
{
public:
	virtual std::string	buildSource() override;
};

#endif