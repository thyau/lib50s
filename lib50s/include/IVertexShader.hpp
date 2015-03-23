#ifndef __IVERTEXSHADER_HPP__
#define __IVERTEXSHADER_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Interface for vertex shader programs.
 */

#include <EShaderTypes.h>

#include "..\\include\\IShader.hpp"

class IVertexShader : public IShader
{
public:
	typedef std::shared_ptr<IVertexShader> Ptr;
	typedef std::weak_ptr<IVertexShader> wPtr;

	virtual irr::video::E_VERTEX_SHADER_TYPE getCompileTarget() = 0;
};

#endif