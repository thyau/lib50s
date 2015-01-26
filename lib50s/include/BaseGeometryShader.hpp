#ifndef __BASEGEOMETRYSHADER_HPP__
#define __BASEGEOMETRYSHADER_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Provides default implementations for some interface functions.
 */

#include "..\\include\\igeometryshader.hpp"

class BaseGeometryShader : public IGeometryShader
{
public:
	virtual IShaderConstantSetCallBack* getDefaultCallbackObject() override	{ return NULL; }
	virtual std::string					getEntryPointName()	override		{ return "main"; }
	virtual E_GEOMETRY_SHADER_TYPE		getCompileTarget() override			{ return EGST_GS_4_0; }
	virtual u32							getMaxVerticesOut() override		{ return 0; }
};

#endif