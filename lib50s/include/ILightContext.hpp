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

#include "..\\include\\Common.hpp"

class ILightContext
{
public:
	virtual const SLight* getLightData() PURE;
	//virtual const SViewFrustum* getViewFrustum() PURE;

	// Needed because SLight.Position is the node's relative position to its parent
	virtual const vector3df& getLightEyePos() PURE;

	// View matrix inverse for the main render camera, not the light camera
	virtual const matrix4& getRenderViewMatrixInverse() PURE;
};

#endif
