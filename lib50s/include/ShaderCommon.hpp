#ifndef __SHADERCOMMON_HPP__
#define __SHADERCOMMON_HPP__

/*
 * Copyright (c) [ORGNAME] 2014
 *
 * Author: Timothy Yau
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Common code definitions used in shader programs. Intended primarily to be
 * included in shader source files only.
 */

#include <iostream>
#include <string>
#include <sstream>

#include "IMaterialRendererServices.h"
#include "ICameraSceneNode.h"
#include "..\\include\\ICameraContext.hpp"

using namespace std;

#define CODE_BEGIN(stream) stream
#define CODE <<endl<<
#define CODE_END <<endl;

namespace shader
{
	// TODO: These function names are pretty horrible! Need some consistent naming scheme.
	// TODO: Where possible, convert to GLSL functions rather than just splicing in variables

	/***** G-buffer functions *****/

	// See DeferredRenderPipeline.cpp for G-Buffer spec
	// colorVar : vec4 RGBA
	// normalVar : vec3 XYZ
	// specLevelVar : float (must be between 0.0 and 255.0)
	// specPowerVar : float
	// depthVar : float
	std::string writeGBuffer(std::string colorVar, std::string normalVar, std::string specLevelVar, std::string specPowerVar, std::string depthVar);

	// Variable types are the same as writeGBuffer. Declares and reads in values into the named variables.
	// coord : vec2 texture coordinate
	std::string readGBuffer(std::string colorVar, std::string normalVar, std::string specLevelVar, std::string specPowerVar, std::string depthVar, std::string coord);

	// uniform declarations for GBuffer textures. Use this at the top of the shader before using readGBuffer.
	std::string includeGBuffer();

	// Use in the shader set constants callback to set the G-Buffer uniforms
	void setGBufferPSConstants(irr::video::IMaterialRendererServices* services);


	/***** Lighting functions *****/

	// coeffs : vec3 with constant, linear, and quadratic attenuation coefficients
	// dist : float distance from scene point to light source
	std::string lightAttenuationFactor(std::string coeffs, std::string dist);

	// L : vec3 normalized vector from scene point toward light
	// V : vec3 normalized vector from scene point toward eye
	std::string blinnPhongBRDF(std::string L, std::string V, std::string surfNorm, std::string specLevel, std::string specPower,std::string outDiffuseCoeff, std::string outSpecCoeff);

	// Requires rescaleDistHeader(), rescaleDistSetConstants(), and includeChebyshevUpperBound()
	std::string vsmShadow(std::string lightDist, std::string shadowMap, std::string shadowTexCoord, std::string outShadowFactor);

	// L : vec3 normalized vector from scene point toward light
	// lightDir : direction that the spot light is facing
	std::string radialAttenuationFactor(std::string L, std::string lightDir, std::string cosInnerAngle, std::string cosOuterAngle, std::string outAttFactor);

	/***** Shadow mapping functions *****/

	// The following are used to rescale a point-to-projection-center distance into the [0,1] range
	// based on the maximum and minimum distance values within the projection frustum. Only for perspective projection.
	std::string rescaleDistHeader();
	void rescaleDistSetConstants(irr::video::IMaterialRendererServices *services, ICameraContext &cameraContext);
	std::string rescaleDist(std::string dist);

	// For a set of floating point numbers with this mean value, subtract the mean when storing, and add back when reading
	// It gives one extra bit of accuracy from the sign
	std::string getFPBias();

	std::string includeChebyshevUpperBound();

	std::string vsmLightBleedReduction(std::string pmax);

	//std::string shadowPCSSHeader();



	/***** Misc functions *****/


	// Perform standard model-view-projection transformation and save the projected point and depth (eye-space) in the specified variables
	// inputVec : vec4 XYZW
	// projOut : vec4 XYZW
	// depthOut : float
	std::string projectAndSaveDepth(std::string inputVec, std::string projOut, std::string depthOut);
}

#endif