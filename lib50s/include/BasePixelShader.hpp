#ifndef __BASEPIXELSHADER_HPP__
#define __BASEPIXELSHADER_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Provides default implementations for some interface functions.
 */

#include "..\\include\\ipixelshader.hpp"

class BasePixelShader : public IPixelShader
{
public:
	virtual IShaderConstantSetCallBack* getDefaultCallbackObject() override	{ return NULL; }
	virtual std::string					getEntryPointName() override		{ return "main"; }
	virtual E_PIXEL_SHADER_TYPE			getCompileTarget() override			{ return EPST_PS_3_0; }
};

#endif