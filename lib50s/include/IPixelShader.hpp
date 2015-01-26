#ifndef __IPIXELSHADER_HPP__
#define __IPIXELSHADER_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Interface for pixel shader programs.
 */

#include "..\\include\\IShader.hpp"

class IPixelShader : public IShader
{
public:
	typedef std::shared_ptr<IPixelShader> Ptr;
	typedef std::weak_ptr<IPixelShader> wPtr;

	virtual E_PIXEL_SHADER_TYPE getCompileTarget() PURE;
};

#endif