#ifndef __BASEVERTEXSHADER_HPP__
#define __BASEVERTEXSHADER_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Provides default implementations for some interface functions.
 */

#include "..\\include\\ivertexshader.hpp"

class BaseVertexShader : public IVertexShader
{
public:
	virtual irr::video::IShaderConstantSetCallBack* getDefaultCallbackObject() override	{ return NULL; }
	virtual std::string								getEntryPointName() override		{ return "main"; }
	virtual irr::video::E_VERTEX_SHADER_TYPE		getCompileTarget() override			{ return irr::video::EVST_VS_3_0; }
};

#endif