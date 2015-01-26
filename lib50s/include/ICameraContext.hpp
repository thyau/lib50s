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

#include "..\\include\\Common.hpp"

class ICameraContext
{
public:
	virtual ICameraSceneNode* getCameraNode() PURE;
	virtual f32 getCameraMinDist() PURE;
	virtual f32 getCameraMaxDist() PURE;
};

#endif
