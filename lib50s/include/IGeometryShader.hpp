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

	virtual E_GEOMETRY_SHADER_TYPE	getCompileTarget() PURE;
	virtual E_PRIMITIVE_TYPE		getGeometryInType() PURE;
	virtual E_PRIMITIVE_TYPE		getGeometryOutType() PURE;
	//  Maximal number of vertices created by geometry shader. If 0, maximal number supported is assumed.
	virtual u32						getMaxVerticesOut() PURE;
};

#endif