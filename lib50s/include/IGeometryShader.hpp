#ifndef __IGEOMETRYSHADER_HPP__
#define __IGEOMETRYSHADER_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Interface for geometry shader programs.
 */

#include "..\\include\\IShader.hpp"

class IGeometryShader : public IShader
{
public:
	typedef std::shared_ptr<IGeometryShader> Ptr;
	typedef std::weak_ptr<IGeometryShader> wPtr;

	virtual irr::video::E_GEOMETRY_SHADER_TYPE	getCompileTarget() = 0;
	virtual irr::scene::E_PRIMITIVE_TYPE		getGeometryInType() = 0;
	virtual irr::scene::E_PRIMITIVE_TYPE		getGeometryOutType() = 0;
	//  Maximal number of vertices created by geometry shader. If 0, maximal number supported is assumed.
	virtual irr::u32							getMaxVerticesOut() = 0;
};

#endif