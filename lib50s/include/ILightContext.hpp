#ifndef __ILIGHTCONTEXT_HPP__
#define __ILIGHTCONTEXT_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Interface to supply lighting data, intended for use in shader callbacks.
 */

#include "vector3d.h"
#include "SLight.h"
#include "matrix4.h"

#include "..\\include\\Common.hpp"

class ILightContext
{
public:
	virtual const irr::video::SLight* getLightData() = 0;
	//virtual const SViewFrustum* getViewFrustum() = 0;

	// Needed because SLight.Position is the node's relative position to its parent
	virtual const irr::core::vector3df& getLightEyePos() = 0;

	// View matrix inverse for the main render camera, not the light camera
	virtual const irr::core::matrix4& getRenderViewMatrixInverse() = 0;
};

#endif
