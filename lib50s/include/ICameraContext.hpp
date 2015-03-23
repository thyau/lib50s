#ifndef __ICAMERACONTEXT_HPP__
#define __ICAMERACONTEXT_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Interface to supply information about a camera, intended to be used in shader callbacks.
 */

#include "ICameraSceneNode.h"
#include "irrTypes.h"

#include "..\\include\\Common.hpp"

class ICameraContext
{
public:
	virtual irr::scene::ICameraSceneNode* getCameraNode() = 0;
	virtual irr::f32 getCameraMinDist() = 0;
	virtual irr::f32 getCameraMaxDist() = 0;
};

#endif
