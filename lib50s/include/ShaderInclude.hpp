#ifndef __SHADERINCLUDE_HPP__
#define __SHADERINCLUDE_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Convenience header to include all shader headers.
 */

// Vertex Shaders
#include "..\\include\\VSNull.hpp"
#include "..\\include\\VSBasic.hpp"
#include "..\\include\\VSNormal.hpp"
#include "..\\include\\VSTangent.hpp"

#include "..\\include\\VSNormCoords.hpp"

// Pixel Shaders
#include "..\\include\\PSNull.hpp"
#include "..\\include\\PSDepth.hpp"
#include "..\\include\\PSDepthMoments.hpp"
#include "..\\include\\PSDistMoments.hpp"

#include "..\\include\\PSDiffuse.hpp"
#include "..\\include\\PSNormalMap.hpp"
#include "..\\include\\PSSpecularMap.hpp"

#include "..\\include\\PSLPoint.hpp"
#include "..\\include\\PSLSpot.hpp"
#include "..\\include\\PSLDirectional.hpp"

#include "..\\include\\PSBoxBlur.hpp"

// Geometry Shaders

#endif