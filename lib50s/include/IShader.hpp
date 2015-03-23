#ifndef __ISHADER_HPP__
#define __ISHADER_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Interface to common to all shader programs. Note that the user may request
 * the shader source or callback multiple times, and these should be assumed
 * to be independent programs with separate allocated resources, if any.
 */

#include <memory>
#include <string>

#include <IShaderConstantSetCallBack.h>

#include "..\\include\\Common.hpp"

class IShader
{
public:
	typedef std::shared_ptr<IShader> Ptr;
	typedef std::weak_ptr<IShader> wPtr;

	// Generate the source code of this shader as a string
	virtual std::string					buildSource			() = 0;

	// Get a newly-allocated callback object for setting shader constants. 
	// User is responsible for drop()'ing it.
	// It is 'default' meaning that it sets constants independently of the other
	// shaders (vertex, pixel, geometry, etc.) in the material.
	// May be NULL if not needed.
	virtual irr::video::IShaderConstantSetCallBack* getDefaultCallbackObject	() = 0;

	// Entry point name in the generated source code
	virtual std::string					getEntryPointName	() = 0;
};

#endif