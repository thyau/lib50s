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

#include "IShaderConstantSetCallBack.h"
#include "IGPUProgrammingServices.h"

#include "..\\include\\ipixelshader.hpp"

class BasePixelShader : public IPixelShader
{
public:
	virtual irr::video::IShaderConstantSetCallBack* getDefaultCallbackObject() override	{ return NULL; }
	virtual std::string								getEntryPointName() override		{ return "main"; }
	virtual irr::video::E_PIXEL_SHADER_TYPE			getCompileTarget() override			{ return irr::video::EPST_PS_3_0; }
};

#endif